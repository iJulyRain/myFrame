local uart_tty = {
	COM1 = "/dev/ttyS1",
	COM2 = "/dev/ttyS2",
}

function getTTY(COMx)
	return uart_tty[COMx]
end

print(getTTY("COM1"))
