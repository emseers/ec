Sometimes with large binaries LFS timeouts before it can upload, therefore increase the lfs time

git config lfs.activitytimeout 600 # Default is 30 (seconds)

This setting can't be stored in .lfsconfig, as git lfs ignores it as a "unsafe" setting :/


Wasn't a problem with S3 backend as it was very fast, Azure Devops is at ~2MB/s >.<