# iisrequestdumper
Plugin for IIS to dump requests and responses to files. Attempts to dump the requests as raw as possible, but beware that at the time the plugin gets called IIS or other plugins may have already altered the requests and ressponse.

To install:

    mkdir c:\tmp\iisRequestDumper
    %systemroot%\system32\icacls "c:\tmp" /grant "everyone":(OI)(CI)F /t
    %systemroot%\system32\icacls "c:\tmp\iisRequestDumper" /grant "everyone":(OI)(CI)F /t

    :: Copy manually iisRequestDumper.dll to d:\

    %systemroot%\system32\inetsrv\appcmd.exe install module /name:iisRequestDumper /image:d:\iisRequestDumper.dll

To remove again:

    %systemroot%\system32\inetsrv\appcmd.exe uninstall module /module.name:iisRequestDumper
    del d:\iisRequestDumper.dll
    del /s /q "c:\tmp\iisRequestDumper" & rmdir /s /q "c:\tmp\iisRequestDumper"

To disable without uninstalling, simply remove or rename the c:\tmp\iisRequestDumper directory.