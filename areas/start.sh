while true; do
  cd /home/fd/fdungeon/areas
  date "+%y%m%d-%H%M%S started" > start.log
  ./rom
done
