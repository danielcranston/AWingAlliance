import sys
import trimesh

filename = sys.argv[1]

a = trimesh.load(filename)
mesh = trimesh.util.concatenate(
    tuple(trimesh.Trimesh(vertices=g.vertices, faces=g.faces)
        for g in a.geometry.values()))
print(mesh.vertices.max(axis=0))
mesh.show()