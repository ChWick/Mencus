DIRS=$(ls -d */)
for i in ${DIRS};
do
    ../../build_linux/bin/MapUpgrader ${i%%/}/${i%%/}.xml
done

./generate_packs.sh
