# Monitor Dimmer

Dim your second monitor if it gets to bright when watching movies, play games, etc.. Compile and run the .exe and the monitor on witch the cursor is, goes dimmer.

### The stroy
I have a dual monitor setup and when I'm full-screen on one of them, the other is really distracting me, both monitors have build in USB hub and just turning the idle one off isn't an option for me, because I lose all my peripherals. So I build this to solve my problem.

The idea was to add a new item on the Desktop right click context menu to run the app quickly. So I found this [powershell script](https://gallery.technet.microsoft.com/scriptcenter/Script-to-add-an-item-to-f523f1f3) online and modified it a little to do this exact thing.

### How to run it
Run PowerShell Console Prompt as Administrator and run the script like this to add the new item to Desktop menu:

```
AddToDesktopContextMenu.ps1 -action "install" -name "the-name-of-your-executable"
```

Or like this to remove it:

```
AddToDesktopContextMenu.ps1 -action "remove" 
```

Make sure when you run the script that it is in the same directory as the executable and the icon.