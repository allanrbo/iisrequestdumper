# iisrequestdumper
Plugin for IIS to dump requests and responses to files. This is useful when troubleshooting IIS issues.

It attempts to dump as raw as possible, but beware that at the time the plugin gets called IIS or other plugins may have already altered the requests and ressponse.

Download: https://github.com/allanrbo/iisrequestdumper/releases/download/1/iisRequestDumper.dll

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