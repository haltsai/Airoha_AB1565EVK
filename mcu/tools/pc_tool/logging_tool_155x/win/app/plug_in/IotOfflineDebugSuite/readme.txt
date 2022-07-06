install step:
1.Run install.bat,
2.Click "Yes" while import reg file,
3.Any key to finish install procedure.
user guide: http://wiki.mediatek.inc/pages/viewpage.action?pageId=165970838


release note:
v0.1813.00
	(1)remove java part(iotOfflineDebugSuite.jar)
	(2)resoved tool crash while parse a long line of log 
	(3)parse TX/RX buffer and control block 
	
v0.1822.00
	(1)option for print heap info
	(2)fix bug: crash if sdk v4.7.0, duo to change bt mm control block struct. 
	
v0.1845.00
	(1)bt memory debug opeion
	(2)bt Rx/Tx Queue parse, before sdk480 only
	(3)2811 dump parse
	(4)Add sdk version and build time check in Keil and IAR 
	
v0.1911.00
	(1)add option for force gen report
		Modify IotLog_Parse.bat install dir, appending string "force" in first line.
		e.g. .\Tool\IotOfflineDebugSuite_v0.1845.00\IotOfflineSST.exe" %1 force  <-- option add
		
