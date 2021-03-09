import matplotlib.pyplot as plt
import matplotlib
import matplotlib.transforms as trsf
import matplotlib.patches as ptc
import numpy as np
from matplotlib.animation import FuncAnimation

#    def __init__(self, ax, problem, result_ij):

def fill(patch, color, alpha=1):
    patch.set_color(color)
    patch.set_alpha(alpha)
    patch.set_edgecolor("black")
    patch.set_linestyle("-")
    patch.set_linewidth(0.7)

def set_transform(ax, patch, tx, ty, angle):
    patch.set_transform(trsf.Affine2D().rotate(angle).translate(tx,ty) + ax.transData)

def add_obstacle(ax, xy, ray):
    ob = ptc.Circle(xy, ray)
    fill(ob, "#E75328")
    ax.add_patch(ob)

def add_capsule(ax, lenght, ray, color, alpha=1):
    points = 200
    def getArc(self, angleS, angleE):
        angles = np.linspace(angleS, angleE, points)
        vertices = np.zeros((points,2) , dtype=np.float64)
        vertices[:,0] = ray * np.cos(angles)
        vertices[:,1] = ray * np.sin(angles)
        return vertices

    vertices = np.zeros((2*points,2) , dtype=np.float64)
    vertices[0:points,:] = getArc(-0.5 * np.pi, 0.5 * np.pi)
    vertices[points:,:] = getArc(0.5 * np.pi,3.0 * np.pi * 0.5)
    vertices[points:,0] = vertices[points:,0] - lenght

    pathPatch = ptc.PathPatch(matplotlib.path.Path(vertices, closed=True))
    fill(pathPatch, color, alpha)
    ax.add_patch(pathPatch)
    return pathPatch

class Manipulator:
    def __init__(self, ax, data):
        dof = (len(data) - 2) / 2
        self.ax = ax
        self.base = [data[0], data[1]]
        self.links = []
        zeroPose = []
        for k in range(0, dof, 1):
            self.links.append({"length":data[2+k], "ray":data[2+dof+k]})
            zeroPose.append(0)
        self.shapes = self.makeCapsuleChain('blue')
        self.setPose(zeroPose, self.links)

    def makeCapsuleChain(self, color, alpha=1):
        chain = []
        for l in self.links:
            chain.append(add_capsule(self.ax, l["length"], l["ray"], color, alpha))
        return chain

    def setPose(self, pose, chain):
        angleCum = 0.0
        posCum = [self.base[0], self.base[1]]
        for p in range(0, len(self.links), 1):
            angleCum = angleCum + pose[p]
            posNew = [ posCum[0] + self.links[p]["length"] * np.cos(angleCum),
                       posCum[1] + self.links[p]["length"] * np.sin(angleCum)]
            set_transform(self.ax, chain[p], posNew[0], posNew[1], angleCum)
            posCum = posNew

    def make_static_pose(self, pose, color):
        chain = self.makeCapsuleChain(color, 0.7)
        self.setPose(pose, chain)
        return

class Scene:
    def __init__(self, fig, ax, problem, solution, trees):
        self.robots = []
        self.solution = []
        for o in problem["obstacles"]:
            add_obstacle(ax, [o[0], o[1]], o[2])
        for r in problem["robots"]:
            self.robots.append(Manipulator(ax, r))
        for s in solution:
            self.solution.append(s)
        # print tree poses
        self.showTree(ax, trees[0], 'blue')
        if(len(trees) > 1):
            self.showTree(ax, trees[1], 'green')
        # enable the animation
		self.animation = FuncAnimation(fig, func=self.setPose, frames=range(0,len(self.solution),1), interval=50, repeat=True)

    def showTree(self, ax, tree, color):
        treePos = np.linspace(0, len(tree), 200).astype(int)
        for t in treePos:
            pos = 0
            for r in self.robots:
                r.make_static_pose(tree[t][pos:pos + len(r.links)], color)
                pos = pos + len(r.links)

    def setPose(self, index):
        pos = 0
        for r in self.robots:
            r.setPose(self.solution[index][pos:pos + len(r.links)])
            pos = pos + len(r.links)

def make_result(fig, ax, problem, result_ij):
    return Scene(fig, ax, problem, result_ij["solution"], result_ij["trees"])