//start "C:\Program Files (x86)\Windows Kits\8.0\Debuggers\x86\windbg" -k com:pipe,port=\\.\pipe\com_1,resets=0,reconnect
//start "C:\Program Files (x86)\Windows Kits\8.0\Debuggers\x86\windbg" -b -k com:pipe,port=\\.\pipe\com_1,resets=0

//start d:\WinDDK\7600.16385.1\Debuggers\windbg -k kdsrv:server=@{tcp:server=192.168.1.118,port=5000},trans=@{com:pipe,port=\\192.168.1.118\pipe\com_1}
//start d:\WinDDK\7600.16385.1\Debuggers\windbg -k kdsrv:server=@{tcp:server=192.168.1.118,port=5000},trans=@{com:pipe,port=\\192.168.1.118\pipe\com_1}
//start d:\WinDDK\7600.16385.1\Debuggers\windbg -b -k com:pipe,port=\\<target_ip or .>\pipe\com_1,resets=0

start C:\WinDDK\7600.16385.1\Debuggers\windbg -b -k com:pipe,port=\\.\pipe\com_2,resets=0