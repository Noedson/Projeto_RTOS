import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from autolabel import autolabel

# Grafico para Intervalo de tempo

# Dados somente para RX..
RX_RTOS_means, RX_RTOS_std = (5.73, 5.84, 6.13), (1.60, 1.83, 2.01)
RX_SemRTOS_means, RX_SemRTOS_std = (10.27, 9.89, 9.54), (3.69, 5.68, 3.37)

# Dados somente para TX
TX_RTOS_means, TX_RTOS_std = (5.04, 5.03, 5.03), (0.18, 0.49, 0.21)
TX_SemRTOS_means, TX_SemRTOS_std = (8.06, 8.00, 8.00), (0.44, 4.59, 0.45)

fig, (ax1, ax2) = plt.subplots(2, 1)

ind = np.arange(len(RX_RTOS_means))  # the x locations for the groups
width = 0.25  # the width of the bars


rects1 = ax1.bar(ind - width/2, RX_RTOS_means, width, yerr=RX_RTOS_std,
                label='with RTOS',color='paleturquoise', hatch="//")
rects2 = ax1.bar(ind + width/2, RX_SemRTOS_means, width, yerr=RX_SemRTOS_std,
                label='without RTOS',color='navajowhite', hatch="x")

# Add some text for labels, title and custom x-axis tick labels, etc.
ax1.set_ylabel('Time interval [s]')
ax1.set_title('99% confidence interval')
ax1.set_xticks(ind)
ax1.set_xticklabels(('Block 1', 'Warehouse', 'Block 6'))
ax1.set_xlabel('Results for RX')
ax1.legend(loc='upper right', ncols=2)
ax1.autoscale_view()

autolabel(rects1, "left")
autolabel(rects2, "right")


rects3 = ax2.bar(ind - width/2, TX_RTOS_means, width, yerr=TX_RTOS_std,
                label='with RTOS',color='paleturquoise', hatch="//")
rects4 = ax2.bar(ind + width/2, TX_SemRTOS_means, width, yerr=TX_SemRTOS_std,
                label='without RTOS',color='navajowhite', hatch="x")


# Add some text for labels, title and custom x-axis tick labels, etc.
ax2.set_ylabel('Time interval [s]')
ax2.set_title('95% confidence interval')
ax2.set_xticks(ind)
ax2.set_xticklabels(('Block 1', 'Warehouse', 'Block 6'))
ax2.set_xlabel('Results for TX')
# ax2.legend(loc='upper right', ncols=2)
ax2.autoscale_view()

autolabel(rects3, "left")
autolabel(rects4, "right")




fig.tight_layout()

plt.savefig(f'RTOS_intervalor_de_tempo.jpg', format='jpg')
plt.savefig(f'RTOS_intervalor_de_tempo.eps', format='eps')
plt.savefig(f'RTOS_intervalor_de_tempo.pdf', format='pdf')
plt.savefig(f'RTOS_intervalor_de_tempo.png', format='png')

plt.show()