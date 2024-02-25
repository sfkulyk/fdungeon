cd $(dirname $0)
ulimit -c unlimited
echo $$ > start.pid
while true; do
  [ -e ../mud/rom ] && cp ../mud/rom .
  date "+%y%m%d-%H%M%S started" >> start.log
  ./rom >> stdout.log 2>> stderr.log
done
rm start.pid
