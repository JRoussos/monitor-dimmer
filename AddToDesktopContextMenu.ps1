#--------------------------------------------------------------------------------- 
#The sample scripts are not supported under any Microsoft standard support 
#program or service. The sample scripts are provided AS IS without warranty  
#of any kind. Microsoft further disclaims all implied warranties including,  
#without limitation, any implied warranties of merchantability or of fitness for 
#a particular purpose. The entire risk arising out of the use or performance of  
#the sample scripts and documentation remains with you. In no event shall 
#Microsoft, its authors, or anyone else involved in the creation, production, or 
#delivery of the scripts be liable for any damages whatsoever (including, 
#without limitation, damages for loss of business profits, business interruption, 
#loss of business information, or other pecuniary loss) arising out of the use 
#of or inability to use the sample scripts or documentation, even if Microsoft 
#has been advised of the possibility of such damages 
#--------------------------------------------------------------------------------- 

[CmdletBinding()]
param ( 
	[Parameter(Mandatory=$true)]
	[string]$action,
	[string]$name = $null
)
$title = "Dim This Screen"
$path = "$PSScriptRoot\$name"
$icon = "$PSScriptRoot\icon_48.ico"

#This function is to add an item to context menu
Function Add-OSCContextItem(	
		[Parameter(Mandatory=$true,Position=0)]
		[String]$DisplayName,
		[Parameter(Mandatory=$true,Position=1)]
		[String]$Argument
	)
{
	New-PSDrive -Name HKCR -PSProvider Registry -Root HKEY_CLASSES_ROOT | Out-Null #Create PSDrive
	$RegistryPath = "HKCR:\DesktopBackground\shell\$DisplayName"

	If(Test-Path -Path $RegistryPath)
	{
		Write-Warning "A same key exists,please use another one."

	}
	Else
	{
		#Modify the registry to add an item to context menu
		New-Item -Path "HKCR:\DesktopBackground\shell" -Name $DisplayName | Out-Null  
		New-Item -Path $RegistryPath -Name "Command" | Out-Null
		Set-ItemProperty -Path $RegistryPath -Name "(Default)" -Value $DisplayName
		Set-ItemProperty -Path $RegistryPath -Name "Icon" -Value $icon
		Set-ItemProperty -Path $RegistryPath"\Command" -Name "(Default)"  -Value $Argument
		If(Test-Path -Path $RegistryPath)
		{
			Write-Host "Add '$DisplayName' to Context successfully."
		}
		Else
		{
			Write-Warning "Failed to add '$DisplayName' to Context successfully "
		}
	}

}

#This function is to delete an item from context menu
Function Remove-OSCContextItem(	
		[Parameter(Mandatory=$true,Position=0)]
		[String]$DisplayName)
{
	New-PSDrive -Name HKCR -PSProvider Registry -Root HKEY_CLASSES_ROOT | Out-Null
	$RegistryPath = "HKCR:\DesktopBackground\shell\$DisplayName"
	If(Test-Path -Path $RegistryPath)
	{	
		#Modify the registry to delete an item from context menu
		Remove-Item -Path $RegistryPath
		If(Test-Path -Path $RegistryPath)
		{
			Write-Warning "Failed to delete '$DisplayName' From Context."
		}
		Else
		{
			Write-Host "Delete '$DisplayName' from Context successfully."
		}	
	}
	Else 
	{
		Write-Warning "Can not find the item: $DisplayName."	
	}
}


If($action -eq "install"){
	If(Test-Path $path -PathType leaf){
		Add-OSCContextItem $title $path
	}Else{
		Write-Warning "Failed to locate '$path'. "
	}
}Elseif($action -eq "remove"){
	Remove-OSCContextItem $title
}Else{
	write-host "Do nothing"
}