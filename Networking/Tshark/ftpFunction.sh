#!/usr/bin/env python
# The purpose of this file is to search in the mypcap2.pcap and extract any PASSWORDS and USERNAMES.
import os
import sys
import subprocess

# Define the tshark command to capture FTP commands
tshark_command = [
    "tshark",
    "-r", "mypcap2.pcap",
    "-Y", "ftp.request.command == USER or ftp.request.command == PASS",  # Filter for FTP USER and PASS commands
    "-T", "fields",  # Output fields
    "-e", "ftp.request.arg",  # Display the command argument (username)
    "-e", "ip.dst" # Find the IP
]
output = subprocess.check_output(tshark_command)
output = output.strip()

# Split the output into lines
output_lines = output.split('\n')

for i in range(0,len(output_lines),2):
	user_line = output_lines[i]
	pass_line = output_lines[i+1]

	username = user_line.split()[0]
	password = pass_line.split()[0]
	ip = user_line.split()[1]

	print "FTP login to ", ip
	print "Username is ", username
	print "Password is ", password
