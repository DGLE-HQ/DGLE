/**
\author Korotkov Andrey aka DRON
\date 15.04.2013 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
-- 3ds Max 2008 and higher is supported.
utility DGLE_Exporter "DGLE Exporter"
(
	struct Vertex(coords, normal, texture)
	-------------------------------------------------------------------------------------
	label lb_Caption "Select what to export:"
	radiobuttons rb_ExportType labels: #("Entire scene", "Selected object(s)", "Skeleton animation") default: 2
	button btn_Export "Export" width:100
	button btn_About "About" width:100
	-------------------------------------------------------------------------------------
	function GetOnlyFileName fileName = 
	(
		cur_name = ""
		
		for i = 1 to fileName.count do
			if fileName[i] == "\\"
				then cur_name = ""
			else
				cur_name = cur_name + fileName[i]
		
		return cur_name
	)
	-------------------------------------------------------------------------------------
	function ExportMesh handleFile meshObj doBuildFasetNormals doOptimize scaleCoeff centerVec txtAppend =
	(
		vertices_all = #()
		progress = 0
		max_progress = meshObj.numfaces
		
		if meshObj.numTVerts == 0 then
			texture_presented = 0
		else
			texture_presented = 1
		
		print "Building triangles..."
		progressStart ("Prepare data... " + txtAppend)
		
		for i = 1 to meshObj.numfaces do
		(
			face = (GetFace meshObj i)
			
			if texture_presented != 0 then
				tex_face = (GetTVFace meshObj i)
			
			for j = 1 to 3 do
			(
				v = (GetVert meshObj face[j])
				
				v += centerVec
				v *= scaleCoeff
				
				n = (GetNormal meshObj face[j])
				
				t_v = point3 0. 0. 0.
				
				if texture_presented != 0 then
					t_v = (GetTVert meshObj tex_face[j])
				
				append vertices_all (Vertex v n (point3 t_v.x ((t_v.y - 1.) * -1.) 0.))
			)
			
			if doBuildFasetNormals then
			(
				work_vertex_1 = vertices_all[i * 3 - 2].coords
				work_vertex_2 = vertices_all[i * 3 - 1].coords
				work_vertex_3 = vertices_all[i * 3].coords
				v_1 = point3 (work_vertex_1.x - work_vertex_2.x) (work_vertex_1.y - work_vertex_2.y) (work_vertex_1.z - work_vertex_2.z)
				v_2 = point3 (work_vertex_2.x - work_vertex_3.x) (work_vertex_2.y - work_vertex_3.y) (work_vertex_2.z - work_vertex_3.z)
				faset_n = cross v_1 v_2
				faset_n = normalize faset_n
				vertices_all[i * 3 - 2].normal = faset_n
				vertices_all[i * 3 - 1].normal = faset_n
				vertices_all[i * 3].normal = faset_n
			)
			
			progress += 1
			
			if not progressUpdate (100. * progress / max_progress) then
				return false
		)
		
		print ("Done, " + (vertices_all.count / 3) as string + " triangles.")
		
		vertices = #()
		indices = #()
		
		progress = 0
		prev_progress = 0
		max_progress = vertices_all.count
		
		print "Building vertices and index buffers..."
		
		if doOptimize then
		(
			progressStart ("Optimize data... " + txtAppend)
			
			for i = 1 to vertices_all.count do
			(
				cur_vertex = vertices_all[i]
				
				j = 1
				
				while j <= vertices.count do
				(
					work_vertex = vertices[j]
					
					if cur_vertex.coords.x == work_vertex.coords.x and cur_vertex.coords.y == work_vertex.coords.y and cur_vertex.coords.z == work_vertex.coords.z and
						cur_vertex.normal.x == work_vertex.normal.x and cur_vertex.normal.y == work_vertex.normal.y and cur_vertex.normal.z == work_vertex.normal.z and
						((texture_presented == 0) or (cur_vertex.texture.x == work_vertex.texture.x and cur_vertex.texture.y == work_vertex.texture.y)) then exit
					
					j += 1
				)
				
				if j == vertices.count + 1 then
					append vertices cur_vertex
				
				append indices j
				
				progress += 1
				
				if not progressUpdate (100. * progress / max_progress) then
					return false
			)
		)
		else
		(
			progressStart ("Building buffers data... " + txtAppend)
			
			for i = 1 to vertices_all.count do
			(
				append vertices vertices_all[i]
				append indices i
				progress += 1
				
				if not progressUpdate (100. * progress / max_progress) then
					return false
			)
		)
		
		print ("Done, " + vertices.count as string + " vertices.")
		
		print "Writing to file..."
		progressStart ("Writing to file... " + txtAppend)
		
		writebyte handleFile texture_presented
		writebyte handleFile 0
		writelong handleFile vertices.count #unsigned 
		writelong handleFile indices.count #unsigned 
		
		writefloat handleFile ((meshObj.center.x + centerVec.x) * scaleCoeff)
		writefloat handleFile ((meshObj.center.y + centerVec.y) * scaleCoeff)
		writefloat handleFile ((meshObj.center.z + centerVec.z) * scaleCoeff)
		
		bounding_box = nodeLocalBoundingBox meshObj
		
		writefloat handleFile (abs(bounding_box[2].x - bounding_box[1].x) / 2. * scaleCoeff)
		writefloat handleFile (abs(bounding_box[2].y - bounding_box[1].y) / 2. * scaleCoeff)
		writefloat handleFile (abs(bounding_box[2].z - bounding_box[1].z) / 2. * scaleCoeff)
		
		for i = 1 to vertices.count do
		(
			writefloat handleFile vertices[i].coords.x
			writefloat handleFile vertices[i].coords.y
			writefloat handleFile vertices[i].coords.z
		)
		
		if not progressUpdate 25 then
			return false
		
		for i = 1 to vertices.count do
		(
			writefloat handleFile vertices[i].normal.x
			writefloat handleFile vertices[i].normal.y
			writefloat handleFile vertices[i].normal.z
		)
		
		if not progressUpdate 50 then
			return false
		
		if texture_presented != 0 then
			for i = 1 to vertices.count do
			(
				writefloat handleFile vertices[i].texture.x
				writefloat handleFile vertices[i].texture.y
			)
		
		if not progressUpdate 75 then
			return false
		
		do_write_long = vertices.count > 65535
		
		for i = 1 to indices.count do
		(
			if do_write_long then
				writelong handleFile (indices[i] - 1) #unsigned
			else
				writeshort handleFile (indices[i] - 1) #unsigned
		)
		
		if not progressUpdate 100 then
			return false
		
		fflush handleFile
		
		print "Done exporting mesh."
		
		return true
	)
	-------------------------------------------------------------------------------------
	function ConvertObjToMesh tryMeshObj =
	(
		if SuperClassOf tryMeshObj != GeometryClass then
		(
			print "Error: Node is not a valid geometry class!"
			return false
		)
		
		if ClassOf tryMeshObj != Editable_mesh then
		(
			if canConvertTo tryMeshObj TriMeshGeometry and yesNoCancelBox ("Convert object \"" + tryMeshObj.name + "\" to editable mesh?") == #yes then
				convertToMesh tryMeshObj
			else
			(
				print "Error: Can't convert to editable mesh!"
				return false
			)
		)
		
		return true
	)		
	-------------------------------------------------------------------------------------
	function GetMeshMaxDemension meshObj =
	(
		bounding_box = nodeLocalBoundingBox meshObj
		
		w = abs(bounding_box[2].x - bounding_box[1].x)
		h = abs(bounding_box[2].y - bounding_box[1].y)
		d = abs(bounding_box[2].z - bounding_box[1].z)
		
		m = w
		if w < h then m = h
		if m < d then m = d
		
		return m
	)
	-------------------------------------------------------------------------------------
	function SaveMeshToFile FileName meshObj doBuildFasetNormals doCenter doScale doOptimize doFrameAnim startFrame endFrame animStep txtAppend =
	(
		disableSceneRedraw()
		
		if not ConvertObjToMesh meshObj then
		(
			enableSceneRedraw()
			return false
		)
				
		print ("Exporting mesh: " + meshObj.name)
		
		scale_c = 1.
		
		if doScale then
		(
			if doFrameAnim then
			(
				cur_frame = startFrame
				
				m = -3.40E38
				idx = 0
				
				while cur_frame <= endFrame do
				(
					slidertime = cur_frame
					
					t = GetMeshMaxDemension meshObj
					
					if t > m then
					(
						idx = cur_frame
						m = t
					)
					
					cur_frame += animStep
				)
				
				slidertime = idx
				scale_c = 1. / m
			)
			else
				scale_c = 1. / (GetMeshMaxDemension meshObj)
		)
		
		center_pos = [0., 0. , 0.]
		
		if doCenter then
			center_pos = [-meshObj.center.x, -meshObj.center.y, -meshObj.center.z]
		
		file = fopen FileName "wb"
		
		if file == undefined then
		(
			print "Error: Couldn't open file for writing !"
			MessageBox "Couldn't open file for writing !"
			return false
		)
		else
		(
			writestring file "DMD 3.1 BIN"
			fseek file 11 #seek_set
			writelong file 0 #unsigned
			writelong file 1 #unsigned
			
			if doFrameAnim then
			(
				cur_frame = startFrame
				
				while cur_frame <= endFrame do
				(
					slidertime = cur_frame
					
					if not ExportMesh file meshObj doBuildFasetNormals doOptimize scale_c center_pos txtAppend then
					(
						fclose file
						progressEnd()
						enableSceneRedraw()
						return false
					)
					
					cur_frame += animStep
				)
			)
			else
				ExportMesh file meshObj doBuildFasetNormals doOptimize scale_c center_pos txtAppend
			
			if doFrameAnim then
			(
				fseek file 15 #seek_set
				writelong file ((endFrame - startFrame) / animStep) #unsigned
			)
			
			fclose file
			
			progressEnd()
		)
		
		print "Mesh export is done."
		enableSceneRedraw()
		
		return true
	)
	-------------------------------------------------------------------------------------
	function ExportSelectedNodes doCenterAndScale doBuildFasetNormals doOptimize doFrameAnim startFrame endFrame animStep =
	(
		if selection.count > 1 and doFrameAnim then
		(
			MessageBox "Animation export for multiple objects is not allowed!"
			return false
		)
		else
			if selection.count == 0 then
			(
				MessageBox "You must select objects in scene to export!"
				return false
			)
		
		print "Model exporting started..."
		
		file_name = GetSaveFileName types: "DMD Models Files (*.dmd)|*.dmd|All Files(*.*)|*.*|"
		
		if file_name == undefined then
		(
			print "Error: Model filename is undefined!"
			MessageBox "Model filename is undefined!"
			return false
		)
		
		if selection.count == 1 then
		(
			if SaveMeshToFile file_name ((selection as array)[1]) doBuildFasetNormals doCenterAndScale doCenterAndScale doOptimize doFrameAnim startFrame endFrame animStep "" then
				print "Model exporting done."
			else
				print "Error: Model exporting failed!"
		)
		else
		(
			disableSceneRedraw()
			
			model_center = [0., 0., 0.]
			model_bounding_box = [0., 0., 0.]
			meshes = #()
			
			max_dem = [-3.40E38, -3.40E38, -3.40E38]
			min_dem = [3.40E38, 3.40E38, 3.40E38]
			
			for node in selection do
				if ConvertObjToMesh node then
				(
					append meshes node
					
					bounding_box = nodeLocalBoundingBox node
					extents = [abs(bounding_box[2].x - bounding_box[1].x) / 2., abs(bounding_box[2].y - bounding_box[1].y) / 2., abs(bounding_box[2].z - bounding_box[1].z) / 2.]
					center = node.center
					
					walls = #([center.x - extents.x, center.y, center.z], [center.x + extents.x, center.y, center.z], [center.x, center.y - extents.y, center.z], [center.x, center.y + extents.y, center.z], [center.x, center.y, center.z - extents.z], [center.x, center.y, center.z + extents.z])
					
					for wall in walls do
					(
						if max_dem.x < wall.x then max_dem.x = wall.x
						if min_dem.x > wall.x then min_dem.x = wall.x
						
						if max_dem.y < wall.y then max_dem.y = wall.y
						if min_dem.y > wall.y then min_dem.y = wall.y
						
						if max_dem.z < wall.z then max_dem.z = wall.z
						if min_dem.z > wall.z then min_dem.z = wall.z
					)
				)
			
			model_center =  min_dem + (max_dem - min_dem) / 2.
			model_bounding_box = max_dem - min_dem
			
			scale_c = 1.
			center_pos = [0., 0., 0.]
			
			if doCenterAndScale then
			(
				m = model_bounding_box.x
				if model_bounding_box.x < model_bounding_box.y then m = model_bounding_box.y
				if m < model_bounding_box.z then m = model_bounding_box.z
				scale_c = 1. / m
				
				center_pos = [-model_center.x, -model_center.y, -model_center.z]
				
				model_center += center_pos
				model_bounding_box *= scale_c
			)
			
			if meshes.count == 0 then
			(
				enableSceneRedraw()
				
				print "Warning: Nothing to export!"
				
				if selection.count != 0 then
					MessageBox "Failed to export model! See MAXSript Listener for details."
				
				return false
			)
			
			skipped = 0
			cnt = 0
			file = fopen file_name "wb"
			
			if file == undefined then
			(
				enableSceneRedraw()
				print "Error: Couldn't open file for writing !"
				MessageBox "Couldn't open file for writing !"
				return false
			)
			else
			(
				writestring file "DMD 3.1 BIN"
				fseek file 11 #seek_set
				writelong file 1 #unsigned --MODEL_AABB
				writelong file meshes.count #unsigned
				
				writefloat file model_center.x
				writefloat file model_center.y
				writefloat file model_center.z
				
				writefloat file (model_bounding_box.x / 2.)
				writefloat file (model_bounding_box.y / 2.)
				writefloat file (model_bounding_box.z / 2.)
				
				for mesh_obj in meshes do
				(
					txt_append = "(" + (cnt + 1) as string + " of " + meshes.count as string + ")"
					
					if not ExportMesh file mesh_obj doBuildFasetNormals doOptimize scale_c center_pos txt_append then
						skipped += 1
					
					cnt += 1
				)
			)
			
			fclose file
			
			progressEnd()
			print ("Model exporting done, " + skipped as string + " meshes of " + selection.count as string + " were skipped due errors.")
			enableSceneRedraw()
		)
		
		return true
	)
	-------------------------------------------------------------------------------------
	rollout roll_SelectedObjects "Selected Objects"
	(
		label lb_SubCaption "Exporting selected object(s)"
		
		checkbox cb_CenterAndScale "Centre and rescale model"
		checkbox cb_BuildFasetNormals "Build faset normals"
		checkbox cb_Optimize "Optimize geometry (slow)"
		
		checkbox cb_DoAnimation "Export frame animation"
		
		label lb_LabelFrames "Frames:"
		spinner sp_Start "From:" range: [animationrange.start, animationrange.end, animationrange.start] type: #Integer
		spinner sp_End "To:" range: [animationrange.start, animationrange.end, animationrange.end] type: #Integer
		spinner sp_Step "Step:" range: [1, animationrange.end, 1] type: #Integer
		
		button btn_Save "Save As..." width:100
		---------------------------------------------------------------------------------
		on roll_SelectedObjects open do
		(
			cb_CenterAndScale.checked = true
			cb_BuildFasetNormals.checked = false
			sp_Start.enabled = false
			sp_End.enabled = false
			sp_Step.enabled = false
			lb_LabelFrames.enabled = false
		)
		---------------------------------------------------------------------------------
		on cb_DoAnimation changed state do
		(
			if cb_DoAnimation.checked then
			(
				sp_Start.enabled = true
				sp_End.enabled = true
				sp_Step.enabled = true
				lb_LabelFrames.enabled = true
			)
			else
			(
				sp_Start.enabled = false
				sp_End.enabled = false
				sp_Step.enabled = false
				lb_LabelFrames.enabled = false
			)
		)
		---------------------------------------------------------------------------------
		on btn_Save pressed do
		(
			DestroyDialog roll_SelectedObjects
			ExportSelectedNodes cb_CenterAndScale.checked cb_BuildFasetNormals.checked cb_Optimize.checked cb_DoAnimation.checked sp_Start.value sp_End.value sp_Step.value
		)
	)
	-------------------------------------------------------------------------------------
	on btn_Export pressed do
	(
		case rb_ExportType.state of
		(
			1: MessageBox "Sorry, not done yet."
			2: createDialog roll_SelectedObjects  modal: true
			3: MessageBox "Sorry, not done yet."
		)
	)
	-------------------------------------------------------------------------------------
	rollout roll_About "About" width:200
	(
		label lb_AboutCaption "DGLE Exporter v 0.3"
		label lb_FormatTxt "Supports DSC, DMD and DSA formats."
		label lb_URL "http://dglengine.org/"
		button btn_Ok "Ok" width: 50
		---------------------------------------------------------------------------------
		on btn_Ok pressed do
			DestroyDialog roll_About
	)
	-------------------------------------------------------------------------------------
	on btn_About pressed do 
		createDialog roll_About modal: true
	-------------------------------------------------------------------------------------
) 