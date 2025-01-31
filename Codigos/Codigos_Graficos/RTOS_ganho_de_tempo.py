import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns


import matplotlib.pyplot as plt
import numpy as np

# Create some fake data.
x = range(0,100)
y1 = np.array([10.7, 10.7, 8, 10.7, 8, 13.4, 11, 7.7, 8.4, 7.7, 11, 10.7, 10.7, 8, 8, 13.4, 8, 8, 8, 10.7, 8, 8, 8, 10.7, 10.7, 8,4, 8, 11, 13.7, 8, 8, 10.7, 13.4, 8, 8, 10.7, 13.4, 8, 8, 10.7, 13.4, 8, 8, 10.7, 10.7, 8, 8, 10.7, 8, 8.7, 8, 10.4, 8, 8.4, 7.7, 13.7, 8, 8, 8, 11, 8, 8.4, 8, 10.7, 13.4, 8, 8, 8, 10.7, 8,4, 8, 8, 13.4, 10.7, 8, 8.4, 10.4, 10.7, 8, 8, 8, 8.4, 8.4, 8, 10.7, 10.7, 8, 8, 8, 8, 8, 10.7, 8, 8, 8, 10.7, 8,4])
y2 = np.array([5, 5, 5, 5, 15, 5, 6.7, 7,4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 10, 5, 5, 5.4, 5, 7, 6.7, 5.7, 5, 5, 5, 5, 6.7, 5, 5, 5, 5, 6.7, 16.4, 5, 5, 5, 5, 7, 6.7, 7, 8, 5.4, 5, 8.4, 6.7, 5, 5, 5, 6.7, 10, 5, 6.7, 5, 5, 5, 6.7, 7, 5, 5, 6.7, 5, 6.7, 6.7, 6.7, 5, 5, 5, 6.7, 8.7, 6.7, 8.4, 5, 5.4, 5, 5, 8.7, 5, 8.4, 6.7, 8.4, 6.7, 5, 5, 5, 5, 8.4, 5, 6.7, 5, 5.4, 5, 5, 6.7, 5, 6.7])
y3 = np.array([3, 3, 3, 12, 2, 12, 2, 4, 3, 2, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 12, 2, 4, 2, 3, 4, 2, 3, 3, 11, 3, 3, 12, 3, 11, 3, 3, 11, 3, 3, 3, 11, 3, 3, 3, 3, 3, 2, 4, 2, 4, 2, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 11, 3, 11, 3, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 5, 10, 4, 2, 11, 4, 3, 3, 3, 2, 4, 2, 3, 3])
z1 = np.array([8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8.3, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7.6, 8, 8, 8, 8, 8, 8, 8, 8.3, 7.6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8.6, 8, 8, 8, 8, 8, 8, 7.6, 8.3, 7.6, 8, 8, 8, 8, 8, 8.6, 8.3, 8, 8, 8, 8, 8, 8, 7.6, 8.3, 8, 8, 8, 8, 8, 8, 8.3, 7,6, 8, 8, 8, 8, 8, 8])
z2 = np.array([5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5.4, 5.4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5.4, 5.4, 5.4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5.4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5.4, 5, 5.4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5.4, 5.4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,4, 5, 5, 5, 5, 5])
z3 = np.array([3, 3, 3, 3, 4, 3, 3, 3, 3, 3, 2.7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2.7, 2.7, 2.7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2.7, 3, 3, 3, 3, 3, 3, 3, 3.4, 2.7, 3, 2.7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2.7, 3, 2.7, 3.7, 3, 3, 3, 3, 3, 3, 2.7, 3.4, 2.7, 3, 3, 3, 3, 3, 3.7, 3.4, 3, 3, 3, 3, 3, 2.7, 2.4, 3.4, 3, 3, 3, 3, 3, 3, 3.4, 2.7, 2.7, 3, 3, 3, 3, 3, 3])



fig, (ax1, ax2) = plt.subplots(2, 1)
# fig.suptitle('A tale of 2 subplots')




ax1.plot(x, y1, '+-', label="RX interval - without RTOS",color='black')
ax1.plot(x, y2, 'x-', label="RX interval - with RTOS",color='darkred')
ax1.plot(x, y3, 'v-', label="Time gain", color='olive')
ax1.set_ylabel('Time(s)')

ax2.plot(x, z1, '+-',label="TX interval - without RTOS",color='black')
ax2.plot(x, z2, 'x-',label="TX interval - with RTOS",color='darkred')
ax2.plot(x, z3, 'v-',label="Time gain",color='olive')
ax2.set_xlabel('Tasks')
ax2.set_ylabel('Time(s)')


ax1.legend()
ax1.autoscale_view()
ax2.legend()
ax2.autoscale_view()



fig.tight_layout()

plt.savefig(f'RTOS_ganho_de_tempo.jpg', format='jpg')
plt.savefig(f'RTOS_ganho_de_tempo.eps', format='eps')
plt.savefig(f'RTOS_ganho_de_tempo.pdf', format='pdf')
plt.savefig(f'RTOS_ganho_de_tempo.png', format='png')

plt.show()
