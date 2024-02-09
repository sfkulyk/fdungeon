cd $(dirname $0)
echo $$ > start.pid
while true; do
  [ -e ../mud/rom ] && cp ../mud/rom .
  date "+%y%m%d-%H%M%S started" >> start.log
  ./rom
done
rm start.pid
