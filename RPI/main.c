#include "installer.h"
#include "net.h"
#include "http.h"
#include "server.h"
#include "util.h"
#include "KPutil.h"

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/SystemService.h>
#include <orbis/ShellUIUtil.h>
#include <orbis/userservice.h>

#define SERVER_PORT (12800)

int sceUserMainThreadPriority = 700;

size_t sceUserMainThreadStackSize = 512 * 1024;
size_t sceLibcHeapSize = 256 * 1024 * 1024;

static bool s_modules_loaded = false;

static bool load_modules(void);
static void unload_modules(void);

static void set_privileges(void);
static void unset_privileges(void);

static void cleanup(void);

typedef struct {
	uid_t uid;
	uid_t ruid;
	uid_t svuid;
	gid_t rgid;
	gid_t svgid;
	uintptr_t prison;
	uintptr_t cdir;
	uintptr_t rdir;
	uintptr_t jdir;
	uint64_t sceProcType;
	uint64_t sonyCred;
	uint64_t sceProcCap;
} jbc_cred;

typedef enum {
	USERSPACE,
	KERNEL_HEAP,
	KERNEL_TEXT
} KmemKind;

enum {
	CWD_KEEP,
	CWD_ROOT,
	CWD_RESET,
};

jbc_cred jailbreak;
jbc_cred og;

int(*jbc_get_cred)(jbc_cred *ans);
int(*jbc_jailbreak_cred)(jbc_cred *ans);
int(*jbc_set_cred)(const jbc_cred *ans);


void Jailbreak()
{
	// unjail
	int urmom = 0;
	int32_t handlejbc = sceKernelLoadStartModule("/app0/sce_module/libjbc.prx", NULL, NULL, NULL, NULL, NULL);
	KernelPrintOut("libjbc handle is 0x%lx\n", handlejbc);

	if (handlejbc > 0)
	{
		urmom = sceKernelDlsym(handlejbc, "jbc_get_cred", (void**)&jbc_get_cred);
		KernelPrintOut("sceKernelDlsym returned 0x%lx\n", urmom);


		urmom = sceKernelDlsym(handlejbc, "jbc_jailbreak_cred", (void**)&jbc_jailbreak_cred);
		KernelPrintOut("sceKernelDlsym returned 0x%lx\n", urmom);

		urmom = sceKernelDlsym(handlejbc, "jbc_set_cred", (void**)&jbc_set_cred);
		KernelPrintOut("sceKernelDlsym returned 0x%lx\n", urmom);
	}
	else
	{
		KernelPrintOut("Failed to load libjbc");
		sceSystemServiceLoadExec((char*)"exit", NULL);
	}

	jbc_get_cred(&og);
	jbc_jailbreak_cred(&jailbreak);
	jbc_set_cred(&jailbreak);
}

int main(int argc, const char* const argv[]) 
{
	char* work_dir;
	char ip_address[16];
	int ret;

	atexit(&cleanup);

	Jailbreak();

	if (!load_modules()) {
		EPRINTF("Unable to load modules.\n");
		goto err;
	}


	//printf("Initializing user service...\n");
	ret = sceUserServiceInitialize(NULL);
	if (ret) {
		EPRINTF("User service initialization failed.\n");
		goto err;
	}

	work_dir = "/data";
	//printf("Working directory: %s\n", work_dir);

	//printf("Initializing AppInstUtil...\n");
	if (!app_inst_util_init()) {
		EPRINTF("AppInstUtil initialization failed.\n");
		goto err_user_service_terminate;
	}

	//printf("Initializing BGFT...\n");
	if (!bgft_init()) {
		EPRINTF("BGFT initialization failed.\n");
		goto err_appinstutil_finalize;
	}

	//printf("Initializing net...\n");
	if (!net_init()) {
		EPRINTF("Net initialization failed.\n");
		goto err_bgft_finalize;
	}

	ret = net_get_ipv4(ip_address, sizeof(ip_address));
	if (ret) {
		EPRINTF("Unable to get IP address: 0x%08X\n", ret);
		goto err_net_finalize;
	}

	//printf("Initializing HTTP/SSL...\n");
	 if (!http_init()) {
	 	EPRINTF("HTTP/SSL initialization failed.\n");
		goto err_net_finalize;
	 }

	//printf("Starting server...\n");
	if (!server_start(ip_address, SERVER_PORT, work_dir)) {
		EPRINTF("Server start failed.\n");
		goto err_http_finalize;
	}

	printf("Listening for incoming connections on %s:%d...\n", ip_address, SERVER_PORT);
	if (!server_listen()) {
		goto err_server_stop;
	}

err_server_stop:
	//printf("Stopping server...\n");
	server_stop();

err_http_finalize:
	//printf("Finalizing HTTP/SSL...\n");
	http_fini();

err_net_finalize:
	//printf("Finalizing net...\n");
	net_fini();

err_bgft_finalize:
	//printf("Finalizing BGFT...\n");
	bgft_fini();

err_appinstutil_finalize:
	//printf("Finalizing AppInstUtil...\n");
	app_inst_util_fini();

err_user_service_terminate:
	//printf("Terminating user service...\n");
	ret = sceUserServiceTerminate();
	if (ret) {
		EPRINTF("sceUserServiceTerminate failed: 0x%08X\n", ret);
	}

err:;

done:
	exit(0);
	return 0;
}

static bool load_modules(void) {
	int ret;

	if (s_modules_loaded) {
		goto done;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSCORE);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYS_CORE), ret);
		goto err;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE), ret);
		goto err_unload_sys_core;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE), ret);
		goto err_unload_system_service;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NETCTL);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NETCTL), ret);
		goto err_unload_user_service;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NET), ret);
		goto err_unload_netctl;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_HTTP);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_HTTP), ret);
		goto err_unload_net;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SSL);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SSL), ret);
		goto err_unload_http;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_APP_INST_UTIL);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_APPINSTUTIL), ret);
		goto err_unload_ssl;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_BGFT), ret);
		goto err_unload_appinstutil;
	}

	ret = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NP_COMMON);
	if (ret) {
		EPRINTF("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NP_COMMON), ret);
		goto err_unload_bgft;
	}

	s_modules_loaded = true;

done:
	return true;

err_unload_np_common:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NP_COMMON);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NP_COMMON), ret);
	}

err_unload_bgft:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_BGFT), ret);
	}

err_unload_appinstutil:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_APP_INST_UTIL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_APPINSTUTIL), ret);
	}

err_unload_ssl:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SSL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SSL), ret);
	}

err_unload_http:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_HTTP);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_HTTP), ret);
	}

err_unload_net:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NET), ret);
	}

err_unload_netctl:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NETCTL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NETCTL), ret);
	}

err_unload_user_service:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE), ret);
	}

err_unload_system_service:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE), ret);
	}

err_unload_sys_core:
	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSCORE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYS_CORE), ret);
	}

err:
	return false;
}

static void unload_modules(void) {
	int ret;

	if (!s_modules_loaded) {
		return;
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NP_COMMON);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NP_COMMON), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_BGFT), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_APP_INST_UTIL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_APPINSTUTIL), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SSL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SSL), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_HTTP);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_HTTP), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NET), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NETCTL);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_NETCTL), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE), ret);
	}

	ret = sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSCORE);
	if (ret) {
		EPRINTF("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", STRINGIFY_DEEP(ORBIS_SYSMODULE_INTERNAL_SYS_CORE), ret);
	}

	s_modules_loaded = false;
}

static void cleanup(void) 
{
	unload_modules();
}

void catchReturnFromMain(int exit_code) {}
