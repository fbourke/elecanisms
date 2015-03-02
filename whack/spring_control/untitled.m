for i = 1:length(flips)
    if flips(i) >= 0
        final(i) = flips(i)*788 + angle(i)
    else
        final(i) = flips(i)*788 + angle(i)
    end
end
close all
plot( linspace(0,2.3,119),final/4.3,'.')
hold all
plot( linspace(0,2.3,119),final/4.3)
xlim([0,2])
xlabel('Time (Seconds)')
ylabel('Displacement (Degrees)')
title('Spring Response')

%%
for i = 1:length(flips)
    if flips(i) >= 0
        final(i) = flips(i)*791 + angle(i);
    else
        final(i) = flips(i)*791 + angle(i);
    end
end
close all
plot( linspace(0,6,1194),final/4.3,'.')
hold all
plot( linspace(0,6,1194),final/4.3)
xlim([0,6])
ylim([-600,800])
line([0,6],[380,380],'k')
line([0,6],[-380,-380],'k')
xlabel('Time (Seconds)')
ylabel('Displacement (Degrees)')
title('Wall Response')