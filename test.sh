MODEM=$(ls /dev/cu.usbmodem* | head -n 1)
echo "Running test on $MODEM"
./test_controller $MODEM
root ./show_plots.C
