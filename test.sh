args=("$@")

if [ $# -gt 0 ]; then
    NAME=${args[0]}
else
    NAME="def"
fi

MODEM=$(ls /dev/cu.usbmodem* | head -n 1)
echo "Running test on $MODEM"
time ./test_controller $MODEM &&
root -l './show_plots.C+ ("'$NAME'")'
