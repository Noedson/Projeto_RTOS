import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from autolabel import autolabel 

# Grafico para Vazão

# Dados somente para RX
RX_RTOS_means, RX_RTOS_std = (164.91, 143.33, 163.18), (22.23, 87.51, 25.92)
RX_SemRTOS_means, RX_SemRTOS_std = (99.42, 98.09, 99.87), (17.99, 18.87, 16.48)

# Dados somente para TX
TX_RTOS_means, TX_RTOS_std = (176.825, 176.76, 178.23), (5.45, 5.74, 6.40)
TX_SemRTOS_means, TX_SemRTOS_std = (110.45, 110.99, 111.97), (5.38, 2.62, 5.91)

fig, (ax1, ax2) = plt.subplots(2, 1)

ind = np.arange(len(RX_RTOS_means))  # the x locations for the groups
width = 0.25  # the width of the bars


rects1 = ax1.bar(ind - width/2, RX_RTOS_means, width, yerr=RX_RTOS_std,
                label='with RTOS',color='paleturquoise', hatch="//")
rects2 = ax1.bar(ind + width/2, RX_SemRTOS_means, width, yerr=RX_SemRTOS_std,
                label='without RTOS',color='navajowhite', hatch="x")

# Add some text for labels, title and custom x-axis tick labels, etc.
ax1.set_ylabel('Data throughput [b/s]')
ax1.set_title('95% confidence interval')
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
ax2.set_ylabel('Data throughput [b/s]')
ax2.set_title('95% confidence interval')
ax2.set_xticks(ind)
ax2.set_xticklabels(('Block 1', 'Warehouse', 'Block 6'))
ax2.set_xlabel('Results for TX')
# ax2.legend(loc='upper right', ncols=2)
ax2.autoscale_view()

autolabel(rects3, "left")
autolabel(rects4, "right")


fig.tight_layout()

plt.savefig(f'RTOS_vazao_dos_tres_cenarios.jpg', format='jpg')
plt.savefig(f'RTOS_vazao_dos_tres_cenarios.eps', format='eps')
plt.savefig(f'RTOS_vazao_dos_tres_cenarios.pdf', format='pdf')
plt.savefig(f'RTOS_vazao_dos_tres_cenarios.png', format='png')

plt.show()