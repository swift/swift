#
# Regular cron jobs for the swift package
#
0 4	* * *	root	[ -x /usr/bin/swift_maintenance ] && /usr/bin/swift_maintenance
