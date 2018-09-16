This directory contains a simple script that allows calling
grive from incron. This will cause any changes you make
to the local directory to automatically call grive to 
update Google Drive. You can copy the script to a location on your 
path:

sudo cp grive-incron /usr/local/bin

To make this work, you also need to insert the contents of incrontab.txt
into your incrontab (after changing the paths to suit your 
installation).

To do this run:
incrontab -e

Then use the editor that appears to insert the line with your custom
paths. You can ignore lines that start with # -- they are just comments
although you can paste them into the incrontab if you like.

To get the reverse, you can do a similar command in your crontab (see
crontab.txt). The example syncs at 5 and 35 minutes after each hour
but you can customize that by changing the crontab line.

crontab -e

will bring up an editor. Copy the line without the # in front from
crontab.txt after fixing it for your specific path names. You can 
skip the # lines or include them, as before.

You probably don't want to do a crontab sync every minute because
grive takes some time to run. The grive-incron script stalls if
another copy of it or grive is running for the current user to 
prevent overrun, but the crontab entry has on such protection. If you
prefer, you could use the same script with a dummy file name in crontab.
Then your crontab line would look something like this:

5,35 * * * * /usr/local/bin/grive-incron DUMMY /home/USER/gdrive

The word DUMMY is just a placeholder. It does not have to refer to a real
file.
