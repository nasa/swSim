import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

dataset = np.load("CrossPly_delam_Z_Slice_results_all.npz")
data = np.real(dataset['Result']) 
# dataset = np.load("Z_Slice_results_2.npz")
# data2 = np.real(dataset['Result'])
# dataset = np.load("Z_Slice_results_3.npz")
# data3 = np.real(dataset['Result'])  


mx = .99*data.max()
mn = .99*data.min()

fig = plt.figure()


# ims is a list of lists, each row is a list of artists to draw in the
# current frame; here we are just animating one artist, the image, in
# each frame
ims = []
for i in range(data.shape[2]):
    im = plt.imshow(data[:,:,i], interpolation='none', animated=True)
    plt.clim([mn, mx])
    plt.axis('off')
    ims.append([im])
# for i in range(100):
#     im = plt.imshow(data2[:,:,i], interpolation='none', animated=True)
#     plt.clim([mn, mx])
#     plt.axis('off')
#     ims.append([im])
# for i in range(60):
#     im = plt.imshow(data3[:,:,i], interpolation='none', animated=True)
#     plt.clim([mn, mx])
#     plt.axis('off')
#     ims.append([im])
plt.colorbar()
ani = animation.ArtistAnimation(fig, ims, interval=50, blit=True,
                                repeat_delay=1000)

ani.save('delam.mp4')

plt.show()