#!/usr/bin/perl -w
use v5.14;
use strict;
use Socket;

# Port number
my $port = shift || 5000;
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
    # Print connection
    my $name = gethostbyaddr($client_addr, AF_INET);
    print "Connection received from $name.\n";

    # Get input
    my $req = <NEW_SOCKET>;
    print("\t" . $name . " : " . $req);
    chomp($req);

    # Switch on input
    given ($req)
    {
        when ("C")
        {
            `python3.7 /home/pi/weather_station/server/pull.py`;
            print NEW_SOCKET `cat /home/pi/weather_station/server/current_conditions`;
        }
        when ("UP")
        {
            print NEW_SOCKET `uptime -p`;
        }
        when ("BBOT")
        {
            print NEW_SOCKET `docker ps --format="{{.Status}}"`;
        }
        default
        {
            print NEW_SOCKET "ERR\n";
        }
    }
    close NEW_SOCKET;
}
