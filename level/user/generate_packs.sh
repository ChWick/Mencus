for i in $(ls -d */); 
do
echo "Generate pack for: " ${i%%/};
cd ${i%%/};
zip -q -u "../${i%%/}" *;
zip -q -u "../${i%%/}" */*;
zip -q -u "../${i%%/}" */*/*;
cd ..;
done
