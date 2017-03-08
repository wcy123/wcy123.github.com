prologues := 2;
beginfig(1);
  N0 := 4;
  N1 := 3;
  N2 := 5;
  N3 := 7;
  pair POINTS[][];
  H = 24cm;
  W = 36cm;
  % draw (0,0)--(W,0)--(W,H)--(0,H)--(0,0);
  for l=0 upto 3:
    for j=0 upto N[l]-1:
      POINTS[l][j] = 0.5[((l+1)/4.0*W,(N[l]-j)/N[l]*H), (l/4.0*W,(N[l]-j-1)/N[l]*H)];
    endfor;
  endfor;

  for l=0 upto 2:
    for i=0 upto N[l]-1:
      for j=0 upto N[l+1]-1:
        drawarrow POINTS[l][i]..POINTS[l+1][j]
          cutbefore fullcircle scaled 2cm shifted POINTS[l][i]
          cutafter fullcircle scaled 2cm shifted POINTS[l+1][j];
        write "label.urt (btex $w_{"&decimal(l)&","&decimal(i)& "," &decimal(j) & "}$ etex scaled 1.2, " &
          "0.15[POINTS["&decimal(l)&"]["&decimal(i)&"],POINTS["&decimal(l+1)&"]["&decimal(j)&"]]);" to "mptextmp.tmp";
      endfor;
    endfor;
  endfor;
  write EOF to "mptextmp.tmp";
  scantokens "input mptextmp.tmp";
  for l=0 upto 3:
    for j=0 upto N[l]-1:
      draw fullcircle scaled 2cm shifted POINTS[l][j];
      draw POINTS[l][j] shifted (0,1cm) -- POINTS[l][j] shifted (0,-1cm);
      write "label (btex $a_{"&decimal(l)&","&decimal(j) & "}$ etex scaled 1.2, " &
        "POINTS["&decimal(l)&"]["&decimal(j)&"]) shifted (0.5cm,0);" to "mptextmp.tmp";
      if l>0:
        write "label (btex $z_{"&decimal(l)&","&decimal(j) & "}$ etex scaled 1.2, " &
          "POINTS["&decimal(l)&"]["&decimal(j)&"]) shifted (-0.5cm,0);" to "mptextmp.tmp";
      fi;
    endfor;
  endfor;
  write EOF to "mptextmp.tmp";
  scantokens "input mptextmp.tmp"
endfig;
end;
