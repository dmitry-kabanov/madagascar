t=0:0.001:1;
T1=2*pi/10;
x1=sin(2*pi*t/T1);
T2=2*pi/40;
x2=sin(2*pi*(t.^4)/T2);

figure
   subplot(2,1,1)
      plot(t,x1,'k')
      axis tight
      grid on
   subplot(2,1,2)
      plot(t,x2,'k')
      axis tight
      grid on

print -depsc junk_ml.eps  
