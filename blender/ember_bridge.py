"""Ember's headless Blender bridge. All paths arrive through a JSON job file."""
import bpy, json, sys, math, base64, tempfile, os
from pathlib import Path
from mathutils import Vector, Matrix

def clean_scene():
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)

def material(name, glow, effect=None):
    mat=bpy.data.materials.new(name);mat.use_nodes=True
    nodes=mat.node_tree.nodes;nodes.clear()
    out=nodes.new('ShaderNodeOutputMaterial');shader=nodes.new('ShaderNodeBsdfPrincipled');color=nodes.new('ShaderNodeAttribute');color.attribute_name='ember_color'
    source=color.outputs['Color']
    if effect and effect.get('anime',False):
        links=mat.node_tree.links;steps=effect.get('animeSteps',3);outline=effect.get('animeOutline',8)/100;brightness=3 if glow else 1
        facing=nodes.new('ShaderNodeLayerWeight');facing.inputs['Blend'].default_value=.5
        inverse=nodes.new('ShaderNodeMath');inverse.operation='SUBTRACT';inverse.inputs[0].default_value=1;links.new(facing.outputs['Facing'],inverse.inputs[1])
        multiply=nodes.new('ShaderNodeMath');multiply.operation='MULTIPLY';multiply.inputs[1].default_value=steps;links.new(inverse.outputs[0],multiply.inputs[0])
        floor=nodes.new('ShaderNodeMath');floor.operation='FLOOR';links.new(multiply.outputs[0],floor.inputs[0])
        scale=nodes.new('ShaderNodeMath');scale.operation='MULTIPLY_ADD';scale.inputs[1].default_value=.65*brightness/(steps-1);scale.inputs[2].default_value=.35*brightness;links.new(floor.outputs[0],scale.inputs[0])
        shade=nodes.new('ShaderNodeMixRGB');shade.blend_type='MULTIPLY';shade.inputs[0].default_value=1;links.new(source,shade.inputs[1]);links.new(scale.outputs[0],shade.inputs[2])
        rim=nodes.new('ShaderNodeMath');rim.operation='LESS_THAN';rim.inputs[1].default_value=outline;links.new(inverse.outputs[0],rim.inputs[0])
        ink=nodes.new('ShaderNodeMixRGB');ink.inputs[2].default_value=(.008,.01,.018,1);links.new(rim.outputs[0],ink.inputs[0]);links.new(shade.outputs[0],ink.inputs[1]);source=ink.outputs[0]
        shader.inputs['Specular IOR Level'].default_value=0
        links.new(source,shader.inputs['Emission Color']);shader.inputs['Emission Strength'].default_value=1
    if effect and effect.get('flipbook'):
        links=mat.node_tree.links;texture=nodes.new('ShaderNodeTexImage')
        with tempfile.NamedTemporaryFile(suffix='.png',delete=False) as temporary:
            temporary.write(base64.b64decode(effect['flipbook']['image'].split(',',1)[1]));filename=temporary.name
        try:
            texture.image=bpy.data.images.load(filename,check_existing=False);texture.image.pack()
        finally:os.unlink(filename)
        multiply=nodes.new('ShaderNodeMixRGB');multiply.blend_type='MULTIPLY';multiply.inputs[0].default_value=1;links.new(source,multiply.inputs[1]);links.new(texture.outputs['Color'],multiply.inputs[2]);source=multiply.outputs[0]
        alpha=nodes.new('ShaderNodeMath');alpha.operation='MULTIPLY';links.new(color.outputs['Alpha'],alpha.inputs[0]);links.new(texture.outputs['Alpha'],alpha.inputs[1]);links.new(alpha.outputs[0],shader.inputs['Alpha'])
        links.new(source,shader.inputs['Emission Color']);shader.inputs['Emission Strength'].default_value=3 if glow else 0
    mat.node_tree.links.new(source,shader.inputs['Base Color'])
    if not (effect and effect.get('flipbook')):mat.node_tree.links.new(color.outputs['Alpha'],shader.inputs['Alpha'])
    if glow and not (effect and (effect.get('anime',False) or effect.get('flipbook'))):
        mat.node_tree.links.new(color.outputs['Color'],shader.inputs['Emission Color']);shader.inputs['Emission Strength'].default_value=3
    shader.inputs['Roughness'].default_value=.65
    mat.node_tree.links.new(shader.outputs['BSDF'],out.inputs['Surface'])
    if hasattr(mat,'surface_render_method'):mat.surface_render_method='DITHERED'
    return mat

def node_group(name, mat, shape='soft', trail=False, mesh_object=None):
    group=bpy.data.node_groups.new(name,'GeometryNodeTree')
    group.interface.new_socket(name='Geometry',in_out='INPUT',socket_type='NodeSocketGeometry');group.interface.new_socket(name='Geometry',in_out='OUTPUT',socket_type='NodeSocketGeometry')
    nodes=group.nodes;links=group.links;src=nodes.new('NodeGroupInput');out=nodes.new('NodeGroupOutput');instances=nodes.new('GeometryNodeInstanceOnPoints');realize=nodes.new('GeometryNodeRealizeInstances');setmat=nodes.new('GeometryNodeSetMaterial');setmat.inputs['Material'].default_value=mat
    primitive=nodes.new('GeometryNodeMeshCylinder' if trail else 'GeometryNodeMeshCube' if shape in ('square','diamond') else 'GeometryNodeMeshCircle' if shape=='ring' else 'GeometryNodeMeshIcoSphere')
    if trail:primitive.inputs['Radius'].default_value=.5;primitive.inputs['Depth'].default_value=1;primitive.inputs['Vertices'].default_value=6
    elif shape in ('square','diamond'):primitive.inputs['Size'].default_value=(2,2,2)
    elif shape=='ring':primitive.inputs['Vertices'].default_value=16;primitive.inputs['Radius'].default_value=1;primitive.fill_type='NGON'
    else:primitive.inputs['Radius'].default_value=1;primitive.inputs['Subdivisions'].default_value=1
    if shape=='star' and not trail and mesh_object is None:
        vertices=[((1 if index%2==0 else .45)*math.cos(index*math.pi/5),(1 if index%2==0 else .45)*math.sin(index*math.pi/5),z) for z in (-.08,.08) for index in range(10)];faces=[tuple(reversed(range(10))),tuple(range(10,20))]+[(index,(index+1)%10,(index+1)%10+10,index+10) for index in range(10)]
        star=bpy.data.meshes.new(name+' star');star.from_pydata(vertices,[],faces);star.update();mesh_object=bpy.data.objects.new(name+' star source',star);bpy.context.scene.collection.objects.link(mesh_object);mesh_object.hide_render=True;mesh_object.hide_set(True)
    if shape=='ring' and not trail and mesh_object is None:
        curve=nodes.new('GeometryNodeCurvePrimitiveCircle');curve.inputs['Resolution'].default_value=16;curve.inputs['Radius'].default_value=1
        profile=nodes.new('GeometryNodeCurvePrimitiveCircle');profile.inputs['Resolution'].default_value=6;profile.inputs['Radius'].default_value=.08
        primitive=nodes.new('GeometryNodeCurveToMesh');links.new(curve.outputs['Curve'],primitive.inputs['Curve']);links.new(profile.outputs['Curve'],primitive.inputs['Profile Curve'])
    if mesh_object is not None:
        primitive=nodes.new('GeometryNodeObjectInfo');primitive.inputs['Object'].default_value=mesh_object
    size=nodes.new('GeometryNodeInputNamedAttribute');size.data_type='FLOAT_VECTOR';size.inputs['Name'].default_value='ember_scale'
    rotation=nodes.new('GeometryNodeInputNamedAttribute');rotation.data_type='FLOAT_VECTOR';rotation.inputs['Name'].default_value='ember_rotation'
    links.new(src.outputs['Geometry'],instances.inputs['Points']);links.new(primitive.outputs['Geometry' if mesh_object is not None else 'Mesh'],instances.inputs['Instance']);links.new(size.outputs['Attribute'],instances.inputs['Scale']);links.new(rotation.outputs['Attribute'],instances.inputs['Rotation']);links.new(instances.outputs['Instances'],realize.inputs['Geometry']);links.new(realize.outputs['Geometry'],setmat.inputs['Geometry']);links.new(setmat.outputs['Geometry'],out.inputs['Geometry'])
    return group

def frame_object(collection,name,points,frame,group,trail=False):
    positions=[];scales=[];rotations=[];colors=[]
    for p in points:
        head=Vector(p[:3]);tail=Vector(p[9:12]);size=p[3]
        if trail:
            delta=head-tail;positions.append((head+tail)*.5);scales.append((max(.003,size*.35),max(.003,size*.35),delta.length));rotations.append(delta.to_track_quat('Z','Y').to_euler() if delta.length else (0,0,0));colors.extend([p[5],p[6],p[7],p[8]*.55])
        else:positions.append(head);scales.append((size,size,size));rotations.append((0,0,p[4]));colors.extend(p[5:9])
    mesh=bpy.data.meshes.new(name);mesh.from_pydata(positions,[],[]);mesh.update()
    for attribute,values in [('ember_scale',scales),('ember_rotation',rotations)]:mesh.attributes.new(attribute,'FLOAT_VECTOR','POINT').data.foreach_set('vector',[number for value in values for number in value])
    mesh.color_attributes.new(name='ember_color',type='FLOAT_COLOR',domain='POINT').data.foreach_set('color',colors)
    obj=bpy.data.objects.new(name,mesh);collection.objects.link(obj);modifier=obj.modifiers.new('Ember particles','NODES');modifier.node_group=group
    animate_frame(obj,frame)
    return obj

def animate_frame(obj,frame):
    # The frame meshes and visibility animation are saved in the .blend file;
    # no runtime handler or automatic Python execution is needed to replay them.
    for time,hidden in [(0,True),(frame,False),(frame+1,True)]:
        obj.hide_viewport=hidden;obj.hide_render=hidden;obj.keyframe_insert(data_path='hide_viewport',frame=time);obj.keyframe_insert(data_path='hide_render',frame=time)

def ribbon_object(collection,name,points,frame,mat,effect):
    vertices=[];faces=[];colors=[]
    for index,p in enumerate(points):
        before=Vector(points[max(0,index-1)][:3]);after=Vector(points[min(len(points)-1,index+1)][:3]);side=(after-before).cross(Vector((0,-1,.5)))
        if side.length<1e-8:side=Vector((1,0,0))
        side.normalize();side*=p[12]*.5;center=Vector(p[:3]);vertices.extend([center-side,center+side]);colors.extend(p[5:9]*2)
        if index:faces.append((index*2-2,index*2-1,index*2+1,index*2))
    mesh=bpy.data.meshes.new(name);mesh.from_pydata(vertices,[],faces);mesh.update();mesh.materials.append(mat)
    mesh.color_attributes.new(name='ember_color',type='FLOAT_COLOR',domain='POINT').data.foreach_set('color',colors)
    obj=bpy.data.objects.new(name,mesh);collection.objects.link(obj);animate_frame(obj,frame);return obj

def flipbook_object(collection,name,points,frame,mat,effect):
    vertices=[];faces=[];colors=[];uvs=[];f=effect['flipbook'];right=Vector((1,0,0));up=Vector((0,.4472136,.8944272))
    for index,p in enumerate(points):
        center=Vector(p[:3]);size=p[3];angle=p[4];r=right*math.cos(angle)+up*math.sin(angle);u=up*math.cos(angle)-right*math.sin(angle);col=int(p[13])%f['columns'];row=int(p[13])//f['columns']
        for x,y in [(-1,-1),(1,-1),(1,1),(-1,1)]:
            vertices.append(center+(r*x+u*y)*size);colors.extend(p[5:9]);uvs.append(((col+(x+1)*.5)/f['columns'],(f['rows']-1-row+(y+1)*.5)/f['rows']))
        faces.append(tuple(index*4+i for i in range(4)))
    mesh=bpy.data.meshes.new(name);mesh.from_pydata(vertices,[],faces);mesh.update();mesh.materials.append(mat);mesh.color_attributes.new(name='ember_color',type='FLOAT_COLOR',domain='POINT').data.foreach_set('color',colors);uv=mesh.uv_layers.new(name='UVMap')
    for loop in mesh.loops:uv.data[loop.index].uv=uvs[loop.vertex_index]
    obj=bpy.data.objects.new(name,mesh);collection.objects.link(obj);animate_frame(obj,frame);return obj

def export_effect(job):
    clean_scene();data=json.loads(Path(job['bake']).read_text(encoding='utf8'));scene=bpy.context.scene;scene.render.fps=data['fps'];scene.frame_start=1;scene.frame_end=len(data['layers'][0]['frames'])
    templates={}
    for reference,filename in job.get('meshes',{}).items():
        previous=set(bpy.data.objects);bpy.ops.import_scene.gltf(filepath=filename);meshes=[obj for obj in bpy.data.objects if obj not in previous and obj.type=='MESH'];bpy.ops.object.select_all(action='DESELECT')
        for obj in meshes:obj.select_set(True)
        if not meshes:raise ValueError('Imported particle mesh is empty.')
        bpy.context.view_layer.objects.active=meshes[0];bpy.ops.object.join();obj=bpy.context.object;obj.data.transform(obj.matrix_world);obj.matrix_world=Matrix.Identity(4);positions=[vertex.co.copy() for vertex in obj.data.vertices];minimum=Vector(tuple(min(point[axis] for point in positions) for axis in range(3)));maximum=Vector(tuple(max(point[axis] for point in positions) for axis in range(3)));center=(minimum+maximum)*.5;extent=max(maximum-minimum)
        if extent<=0:raise ValueError('Particle mesh has no dimensions.')
        obj.data.transform(Matrix.Scale(2/extent,4) @ Matrix.Translation(-center));obj.name='Ember source mesh '+reference[:8];obj.hide_render=True;obj.hide_set(True);templates[reference]=obj
    for layer in data['layers']:
        collection=bpy.data.collections.new(layer['name']);scene.collection.children.link(collection);mat=material('Ember '+layer['name'],layer['effect']['glow'],{key:value for key,value in layer['effect'].items() if key!='flipbook' or layer['effect'].get('renderer')!='mesh'});head=node_group('Ember '+layer['name'],mat,layer['effect']['sprite'],mesh_object=templates.get(layer['effect'].get('meshRef')));tail=node_group('Ember trail '+layer['name'],mat,trail=True) if layer['effect']['trail'] and layer['effect'].get('trailMode')!='ribbon' else None
        strip_effect={key:value for key,value in layer['effect'].items() if key!='flipbook'};strip_mat=material('Ember strip '+layer['name'],strip_effect['glow'],strip_effect) if layer['effect'].get('flipbook') else mat
        if tail:tail=node_group('Ember plain trail '+layer['name'],strip_mat,trail=True)
        for index,points in enumerate(layer['frames']):
            if layer['effect'].get('flipbook') and layer['effect'].get('renderer')!='mesh':flipbook_object(collection,'Ember texture '+layer['name']+' '+str(index+1),points,index+1,mat,layer['effect'])
            else:frame_object(collection,'Ember '+layer['name']+' '+str(index+1),points,index+1,head)
            if layer['effect']['trail'] and layer['effect'].get('trailMode')=='ribbon':ribbon_object(collection,'Ember ribbon '+layer['name']+' '+str(index+1),points,index+1,strip_mat,layer['effect'])
            if tail:frame_object(collection,'Ember trail '+layer['name']+' '+str(index+1),points,index+1,tail,True)
    text=bpy.data.texts.new('Ember Original Project.json');text.write(json.dumps(data['project'],indent=2));scene['ember_bake']=True;scene['ember_units']='meters';scene.frame_set(min(2,scene.frame_end));bpy.ops.wm.save_as_mainfile(filepath=job['output'])

def load_file(filename):
    extension=Path(filename).suffix.lower()
    if extension=='.blend':bpy.ops.wm.open_mainfile(filepath=filename,load_ui=False,use_scripts=False)
    else:
        clean_scene()
        if extension=='.glb':bpy.ops.import_scene.gltf(filepath=filename)
        elif extension=='.fbx':bpy.ops.import_scene.fbx(filepath=filename)
        else:raise ValueError('Choose a .blend, .glb, or .fbx mesh file.')

def export_mesh(job):
    load_file(job['input']);objects=[obj for obj in bpy.context.scene.objects if obj.type=='MESH' and obj.visible_get()]
    if not objects:raise ValueError('The file has no visible mesh objects at its current frame.')
    bpy.ops.object.select_all(action='DESELECT')
    for obj in objects:obj.select_set(True)
    bpy.context.view_layer.objects.active=objects[0]
    bpy.ops.export_scene.gltf(filepath=job['glb'],export_format='GLB',use_selection=True,export_animations=False,export_skins=False,export_apply=True)
    bpy.ops.export_scene.fbx(filepath=job['fbx'],use_selection=True,object_types={'MESH'},bake_anim=False,use_mesh_modifiers=True,add_leaf_bones=False)
    if job.get('output'):bpy.ops.wm.save_as_mainfile(filepath=job['output'])

def recover_effect(job):
    bpy.ops.wm.open_mainfile(filepath=job['input'],load_ui=False,use_scripts=False);text=bpy.data.texts.get('Ember Original Project.json')
    if not text:raise ValueError('This .blend file does not contain an original Ember project.')
    Path(job['output']).write_text(text.as_string(),encoding='utf8')

job=json.loads(Path(sys.argv[sys.argv.index('--')+1]).read_text(encoding='utf8'))
try:
    {'export_effect':export_effect,'export_mesh':export_mesh,'recover_effect':recover_effect}[job['action']](job)
    print('EMBER_BLENDER_SUCCESS')
except Exception as error:
    print('EMBER_BLENDER_FAILED '+str(error));sys.exit(1)
