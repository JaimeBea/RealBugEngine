@echo off

powershell Remove-Item -Path "Build/Package" -ErrorAction "Ignore" -Recurse
powershell New-Item -Path "Build/Package" -ItemType "Directory" -Force
powershell New-Item -Path "Build/Package/RealEngine" -ItemType "Directory" -Force
powershell Copy-Item -Path "Licenses", "LICENSE", "README.md" -Destination "Build/Package" -Force -Recurse
powershell Copy-Item -Path "Game/*", "Build/Release/*.exe" -Destination "Build/Package/RealEngine" -Force -Recurse
powershell Compress-Archive -Path "Build/Package/*" -DestinationPath "Build/RealEngine.zip" -Force

pause

:exit