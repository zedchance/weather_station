#!/usr/bin/perl -w

use strict;
use Socket;

# Port number
my $port = 5000;
my $proto = getprotobyname('tcp');
my $server = "192.168.50.84";

# Create a socket
socket(SOCKET, PF_INET, SOCK_STREAM, $proto)
    or die "Can't open socket $!\n";
setsockopt(SOCKET, SOL_SOCKET, SO_REUSEADDR, 1)
    or die "Can't set socket option to SO_REUSEADDR $!\n";

# Bind to port and listen
bind(SOCKET, pack_sockaddr_in($port, inet_aton($server)))
    or die "Can't bind to port $port!\n";
listen(SOCKET, 5) or die "Listen: $!\n";
print "WEATHER_SERVER started on $server $port.\n";

# Accept incoming connection
my $client_addr;
while ($client_addr = accept(NEW_SOCKET, SOCKET))
{
    # Pull new weather and respond
    `python3.7 /home/pi/weather_station/server/pull.py`;
    my $name = gethostbyaddr($client_addr, AF_INET);
    print NEW_SOCKET `cat /home/pi/weather_station/server/current_conditions`;
    print "Connection received from $name.\n";
    close NEW_SOCKET;
}
