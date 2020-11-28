${EngineName} = "RealEngine"
${Version} = "0.0.1"

${ReleaseDir} = "Build/Release"
${GameDir} = "Game"
${LicensesDir} = "Licenses"

${PackageDir} = "Build/Package"
${OutDir} = "Build"

Remove-Item -Path "${PackageDir}" -ErrorAction "Ignore" -Recurse
New-Item -Path "${PackageDir}" -ItemType "Directory" -Force
New-Item -Path "${PackageDir}/${EngineName}" -ItemType "Directory" -Force
Copy-Item -Path "${LicensesDir}", "LICENSE", "README.md" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "${GameDir}/*", "${ReleaseDir}/*.exe" -Exclude "*.ini" -Destination "${PackageDir}/${EngineName}" -Force -Recurse
Compress-Archive -Path "${PackageDir}/*" -DestinationPath "${OutDir}/${EngineName}_${Version}.zip" -Force