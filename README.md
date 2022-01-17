# ps4_remote_pkg_installer-OOSDK
This is a OOSDK port of flat's ps4_remote_pkg_installer, all credit goes to them and their amazing work! 

# NOTES
- Some functions are not in OOSDK yet(:tm:) so i worked around this by manually importing them.
- Some freeBSD net functions where giving issue with printing debug info out, so i swapped these to the sceNet* versions as they seemed to work.
- if you have ran the orginal remote package installer it will cause a port conflict and this one will fail to function, restart your unit and only run this
- WINDOWS building only for the time being 