mkdir -p sources
cd sources
git clone --recurse-submodules git@github.com:artem78/s60-maps.git
cd s60-maps
# Get latest annotated tag (release)
TagName=$(git describe --abbrev=0)
echo "Current program version: $TagName"
git checkout $TagName
rm -r -f .git
zip ../s60-maps_complete_sources_$TagName.zip . -r
cd ..
rm -r -f s60-maps
