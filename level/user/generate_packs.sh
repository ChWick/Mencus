echo "Removing temporary files"
find . -type f \( -name '*.swp' -o -name '*~' -o -name '*.bak' -o -name '.netrwhist' \) -delete

for i in $(ls -d */); 
do
echo "Generate pack for: " ${i%%/};
cd ${i%%/};
zip -q -u "../${i%%/}" *;
zip -q -u "../${i%%/}" */*;
zip -q -u "../${i%%/}" */*/*;
cd ..;
done
