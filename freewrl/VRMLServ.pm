
# $Id: VRMLServ.pm,v 1.5 2000/10/13 14:13:48 crc_canada Exp $
#
# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.

# Implement communication with EAI and script processes.

#
# $Log: VRMLServ.pm,v $
# Revision 1.5  2000/10/13 14:13:48  crc_canada
# removed some unneeded print statements
#
# Revision 1.4  2000/10/12 12:18:20  crc_canada
# fixed a send event type mfnode bug, whereby two events in succession would
# destroy the preceeding.
#
# Revision 1.3  2000/09/22 13:55:24  crc_canada
# Fixed bugs where EAI would hang if appletviewer opened after freewrl. Also
# making changes in EAI code - all new java files in vrml/external needed.
#
# Revision 1.2  2000/08/05 11:56:56  rcoscali
# Add CVS keywords
#
#

package VRML::EAIServer;
use FileHandle;
use IO::Socket;
use strict;

# EAIhost and EAIport are passed in to freewrl as command line parameters.
my $EAIhost = "";
my $EAIport = 0;

# EAIrecount  is used for when a connection is requested, but is not
# opened. This is a retry counter.
my $EAIrecount = 0;

sub new {
	my($type,$browser) = @_;
	my $this = bless {
		B => $browser,
	}, $type;
	$browser->add_periodic(sub {$this->poll});
	return $this;
}

sub gulp {
	my($this, $handle) = @_;
	my ($s,$b);
	my($rin,$rout);
	do {
		# print "GULPING\n";
		my $n = $handle->sysread($b,1000);
		# print "GULPED $n ('$b')\n";
		goto OUT if !$n;
		$s .= $b;
		vec($rin,$handle->fileno,1) = 1;
		select($rout=$rin,"","",0);
		# print "ROUT : $rout\n";
	} while(vec($rout,$handle->fileno,1));
	# print "ENDGULP\n";
  OUT:
	return $s;
}

# However funny this is, we do connect as the client ;)
sub connect {
	my($this, $addr) = @_;
	$addr =~ /^(.*):(.*)$/ or 
		die  "Invalid EAI adress '$addr'";
	
	($EAIhost, $EAIport) = ($1,$2);

	print ("FreeWRL: connect: remote $EAIhost  port $EAIport\n");
	my $sock;
	$sock = IO::Socket::INET->new(
		Proto => "tcp",
		PeerAddr => $EAIhost,
		PeerPort => $EAIport
	);

	# is socket open? If not, wait.....
	if (!$sock) { 
		print "FreeWRL: Connect: socket not opened yet...\n";
		return;
	}

	$this->doconnect($sock);
}

sub doconnect {
	my($this,$sock) = @_;

	$sock->autoflush(1);
	$sock->setvbuf("",&_IONBF,0);
	$sock->print("FreeWRL EAI Client 0.27\n");
	my $x;
	$sock->sysread($x,20); 
	chomp $x;
	if("FreeWRL EAI Serv0.27" ne $x) {
		warn("EAI Version Mismatch! got $x");
	}
	push @{$this->{Conn}}, $sock;
	
}

sub poll {
	my($this) = @_;
	my ($nfound, $timeleft,$rout);


	# if the socket is not open yet, try it, once again...
	if (!defined $this->{Conn}) {
		# print "FreeWRL: Poll: socket not opened yet for host $EAIhost port $EAIport\n";
		# lets just try again in a while...
		if ($EAIrecount < 100) {
			$EAIrecount +=1;
			return;
		}

		# woops! While is up! lets try connecting again.
        	my $sock;
       		 $sock = IO::Socket::INET->new(
       		         Proto => "tcp",
       		         PeerAddr => $EAIhost,
       		         PeerPort => $EAIport
       		 );

       		 # is socket open? If not, wait.....
    		    if (!$sock) {
			 $EAIrecount = 0;
       		         #print "FreeWRL: Poll: socket not opened yet...\n";
       		 } else {
			print "FreeWRL: Poll: Socket finally opened!!! \n";
        		$this->doconnect($sock);
		}

	}

	if (defined $this->{Conn}) {
		my $rin = '';

	
		for(@{$this->{Conn}}) {
			vec($rin, $_->fileno, 1) = 1;
		}
		($nfound, $timeleft) = select($rout = $rin, '', '', 0);
		# print "SELECT NF $nfound\n";
		if($nfound) {
			for(@{$this->{Conn}}) {
				if(vec($rout, $_->fileno, 1)) {
					# print "CONN: $_\n";
					$this->handle_input($_);
				}
			}
		}
	}
}

sub handle_input {
	my($this, $hand) = @_;

	my @lines = split "\n",$this->gulp($hand);

	while(@lines) {
		if($VRML::verbose::EAI) {
		  print "Handle input $#lines\nEAI input:\n";
		  my $myline; 
		  foreach $myline (@lines) {
			print "....",$myline,".... \n";
		  }
		  print ".. finished\n\n";
		}

		my $reqid = shift @lines; # reqid + newline

                if ($reqid eq '') {
		  $reqid = shift @lines; # reqid + newline
                }

		my $str = shift @lines; 

		if($str =~ /^GN (.*)$/) { # Get node
			my $node = $this->{B}->api_getNode($1);
			my $id = VRML::Handles::reserve($node);

                        # remember this - this node is displayed already
                        VRML::Handles::displayed($node);

			if($VRML::verbose::EAI) {
	                  print "GN returns $id\n";
       		        }
		        $hand->print("RE\n$reqid\n1\n$id\n");

		} elsif($str =~ /^GFT ([^ ]+) ([^ ]+)$/) { # get field type & kind
			my($id, $field) = ($1, $2);
			my ($kind, $type) = 
			 $this->{B}->api__getFieldInfo(VRML::Handles::get($id),
				$field);
			if($VRML::verbose::EAI) {
	                  print "GFT returns $kind $type\n";
       		        }
		        $hand->print("RE\n$reqid\n2\n$kind\n$type\n");

		} elsif($str =~ /^GI ([^ ]+) ([^ ]+)$/) { # get eventIn type
			my($id, $field) = ($1, $2);
			my ($kind, $type) = 
			 $this->{B}->api__getFieldInfo(VRML::Handles::get($id),
				$field);
			if($VRML::verbose::EAI) {
	                  print "GI returns $type\n";
       		        }
		        $hand->print("RE\n$reqid\n1\n$type\n");

		} elsif($str =~ /^GT ([^ ]+) ([^ ]+)$/) { # get eventOut type
			my($id, $field) = ($1, $2);
			my $node = VRML::Handles::get($id);
			my ($kind, $type) = 
			 $this->{B}->api__getFieldInfo($node, $field);
		        $hand->print("RE\n$reqid\n0\n$type\n");

		} elsif($str =~ /^GV ([^ ]+) ([^ ]+)$/) { # get eventOut Value
			my($id, $field) = ($1, $2);
			my $node = VRML::Handles::get($id);
			my ($kind, $type) = 
			 $this->{B}->api__getFieldInfo($node, $field);
			my $val = $node->{RFields}{$field};
			if ($val eq '') {
				my $val = $node->{Fields}{$field};
			}

			my $strval = "VRML::Field::$type"->as_string($val);
			if ($type eq "MFNode") {
				# if this is a MFnode, we don't want the VRML CODE
				$strval = $node;
			}
			if($VRML::verbose::EAI) {
	                  print "GV returns $strval\n";
       		        }
		        $hand->print("RE\n$reqid\n2\n$strval\n");

		} elsif($str =~ /^SC ([^ ]+) ([^ ]+)$/) { # send SFNode eventIn to node
			my($id, $field) = ($1,$2);
			my $v = (shift @lines)."\n";

		        # JS - sure hope we can remove the trailing whitespace ALL the time...
  			$v =~ s/\s+$//; # trailing new line....

			my $node = VRML::Handles::get($id);
			my $child = VRML::Handles::get($v);


			# the events are as follows:
			# VRMSServ.pm - api__sendEvent(
			#	handle VRML::Handles::get($id); 
			#	"children"
			#	 array VRML::Handles::get($v) + previous children
			#
			# Browser.pm:api__sendEvent(
			#	->{EV}->send_event_to (same parameters)
			#	   ie, node, field, value
			#
			# Events.pm:send_event_to(
			#	push on {ToQueue}, [parameters] 
			#
			# then, some time later....
			# Browser.pm:tick calls
			# Events.pm:propagate_events sends this eventually to
			#
			# Scene.pm:receive_event (this, field, value...)
			# Tuomas' comments follow:	
			# The FieldHash
			#
			# This is the object behind the "RFields" hash member of
			# the object VRML::Node. It allows you to send an event by
			# simply saying "$node->{RFields}{xyz} = [3,4,5]" for which
			# calls the STORE method here which then queues the event.
			#
			# so, 
			# Scene.pm:STORE (node, "children" value)
			#	$$v = $value;
			# 	$node->set_backend_fields ("children");
			#
			# Scene.pm:set_backend_fields (field)
			#	calls make_backend for $v
			#	takes the global $v, creates a global $f{"children"}=$v, 
			#	and calls
			#	$be->set_fields($this->{BackNode},\%f);
			#	
			# and the backend sets the fields, and everyone lives happily
			# ever after...
			#	

			if($field eq "addChildren") {
				# is the child already there???
				if (!($this->{B}->checkChildPresent($node,$child))) {
					my @av = @{$node->{RFields}{children}};
					push @av, $child;
		  			$this->{B}->api__sendEvent($node, "children",\@av);
				}
			} else {
		  		print "VRMLServ.pm, 2\n";
				if ($this->{B}->checkChildPresent($node,$child)) {
		  			my @av = $this->{B}->removeChild($node, $child);
		  			$this->{B}->api__sendEvent($node, "children",\@av);
				}
			}
		        $hand->print("RE\n$reqid\n0\n0\n");
			print "VRMLServ.pm - done SC\n";
		} elsif($str =~ /^SE ([^ ]+) ([^ ]+)$/) { # send eventIn to node
			my($id, $field) = ($1,$2);
			my $v = (shift @lines)."\n";
                        
		        # JS - sure hope we can remove the trailing whitespace ALL the time...
  			$v =~ s/\s+$//; # trailing new line....

			my $node = VRML::Handles::get($id);
			my $ft = $node->{Type}{FieldTypes}{$field};

			# JAS my ($x,$FieldType) = $this->{B}->api__getFieldInfo($node,$field);
			# JAS print "VRMLServ.pm: for node $node, ft $ft, $x, $FieldType\n";
			if ($ft eq "SFNode"){
				# print "VRMLServ.pm - doing a SFNode\n";
				my $child = VRML::Handles::get($v);
				# print "VRMLServ.pm, ft $ft child $child\n";
			    	$this->{B}->api__sendEvent($node, $field, $child);
			} else {
		    		# print "VRMLServ.pm, 3\n";
			    	my $value = "VRML::Field::$ft"->parse("FOO",$v);
		    		$this->{B}->api__sendEvent($node, $field, $value);
			}

		} elsif($str =~ /^DN (.*)$/) { # Dispose node
			VRML::Handles::release($1);
		        $hand->print("RE\n$reqid\n0\n");

		} elsif($str =~ /^RL ([^ ]+) ([^ ]+) ([^ ]+)$/) {
			my($id, $field, $lid) = ($1,$2,$3);
			my $node = VRML::Handles::get($id);
			$this->{B}->api__registerListener(
				$node,
				$field,
				sub {
					$this->send_listened($hand,
						$node,$id,$field,$lid,
						$_[0]);
				}
			);
		        $hand->print("RE\n$reqid\n0\n0\n");

		} elsif($str =~ /^GNAM$/) { # Get name
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->getName(), "\n");

		} elsif($str =~ /^GVER$/) { # Get Version
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->getVersion(), "\n");

		} elsif($str =~ /^GCS$/) { # Get Current Speed
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->getCurrentSpeed(), "\n");

		} elsif($str =~ /^GCFR$/) { # Get Current Frame Rate
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->getCurrentFrameRate(), "\n");

		} elsif($str =~ /^GWU$/) { # Get WorldURL
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->getWorldURL(), "\n");

		} elsif($str =~ /^RW$/) { # replaceWorld
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->replaceWorld(), "\n");

		} elsif($str =~ /^LU$/) { # LoadURL
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->loadURL(), "\n");

		} elsif($str =~ /^SD$/) { # set Description
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->setDescription(), "\n");

		} elsif($str =~ /^CVS (.*)$/) { # Create VRML From String
			my $vrmlcode = $1;
			my $ll = "";
			while ($ll ne "EOT") {
			  $vrmlcode = "$vrmlcode $ll\n";
                          $ll = shift @lines;
			} 

		        $hand->print("RE\n$reqid\n0\n", $this->{B}->createVrmlFromString($vrmlcode), "\n");

		} elsif($str =~ /^CVU (.*)$/) { # Create VRML From URL
		        $hand->print("RE\n$reqid\n0\n", $this->{B}->createVrmlFromURL($1,$2), "\n");


		} else {
			if ($str ne  "") {
				die("Invalid EAI input: '$str'");
			}
		}
	}
	# print "ENDLINES\n";
}

sub send_listened {
	my($this, $hand, $node, $id, $field, $lid, $value) = @_;
	my $ft = $node->{Type}{FieldTypes}{$field};
	my $str = "VRML::Field::$ft"->as_string($value);

	$VRML::EAIServer::evvals{$lid} = $str;
	$hand->print("EV\n"); # Event incoming
	$hand->print("$lid\n");
	$hand->print("$str\n");
}


1;
