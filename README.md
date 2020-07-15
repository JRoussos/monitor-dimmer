# Monitor Dimmer


Dim your second monitor if it gets to bright when watching movies, play games, etc.. Compile and run the .exe and the monitor on witch the cursor is, goes dimmer.

The idea was to add a new item on the Desktop's right click context menu to run the app quickly. So I found this [powershell script](https://gallery.technet.microsoft.com/scriptcenter/Script-to-add-an-item-to-f523f1f3) online and modified it a little to do that exact thing.


Run PowerShell Console Prompt as Administrator and run the script like this to add the new item to Desktop's menu:

```
AddToDesktopContextMenu.ps1 -action "install" 
```

Or like this to remove it:

```
AddToDesktopContextMenu.ps1 -action "remove" 
```

Make sure when you run the script that it is in the same directory as the exectutable and the icon.