--DGLE2 Models and Scenes converter V Beta 1
--(c)DRON 2010 (http://dron.deeprosoft.com/dgle2)
utility MExport "DGLE Exporter"
(
	
	struct 	vertex(coord, normal, texcoord)
	struct	mesh_info(vcount, fcount, textured)
	
	local
	center_and_scale, build_faset, gm_optimize,
	do_frame_anim, start_frame, end_frame, anim_step,
	export_txt_append
	
	-------------------------------------------------------------------------------------
	label whtexp "Select what to export:"	
	radiobuttons rb_ExScene labels:#("Entire scene","Selected objects only") default:2
	button btn_export "Export" width:100
	button btn_about "About" width:100
	-------------------------------------------------------------------------------------
	function GetFName fname = 
	(
		curname = ""
		for i = 1 to fname .count do
		if fname[i]=="\\" then curname = "" else curname=curname+fname[i]
		return curname
	)
	-------------------------------------------------------------------------------------
	function ExportMesh File meshObj buildFNormals scaleKoeff centerVec=
	(	
		vertices_all 	= #()
			
		progress 		= 0
		max_progress 	= meshObj.numfaces
		
		if meshObj.numTVerts==0 then
			texture_presented = 0
		else
			texture_presented = 1
		
		print "Building triangles..."
		progressStart ("Prepare data... " + export_txt_append)

			for i = 1 to meshObj.numfaces do
			(
				f  = (GetFace meshObj i)
				if texture_presented!=0 then
					tf = (GetTVFace meshObj i)
				
				for j = 1 to 3 do
				(
						v 	= (GetVert meshObj f[j])
						v	+= centerVec
						v	*= scaleKoeff
					
						n 	= (GetNormal meshObj f[j])
		
						tv = point3 0. 0. 0.
						if texture_presented!=0 then
							tv = (GetTVert meshObj tf[j])
												
						append vertices_all (vertex v n (point3 tv.x ((tv.y-1.)*-1.) 0.))
				)
				
				if buildFNormals then
				(
					wrkVertx1 = vertices_all[i*3-2].coord
					wrkVertx2 = vertices_all[i*3-1].coord
					wrkVertx3 = vertices_all[i*3].coord
					v1 = point3 (wrkVertx1.x - wrkVertx2.x) (wrkVertx1.y - wrkVertx2.y) (wrkVertx1.z - wrkVertx2.z)
					v2 = point3 (wrkVertx2.x - wrkVertx3.x) (wrkVertx2.y - wrkVertx3.y) (wrkVertx2.z - wrkVertx3.z)
					faset_n = cross v1 v2
					faset_n = normalize faset_n
					vertices_all[i*3-2].normal 	= faset_n
					vertices_all[i*3-1].normal 	= faset_n
					vertices_all[i*3].normal 	= faset_n
				)

				progress+=1
				if not progressUpdate (100.*progress/max_progress) then return mesh_info 0 0 0
			)			

		print ("Done. " + vertices_all.count as string + " triangles")	
			
		vertices	= #()			
		indexes	= #()
			
		progress 		= 0
		prev_progress	= 0
		max_progress 	= vertices_all.count
		
		print "Building vertices and index buffers..."

		if gm_optimize then
		(
			progressStart ("Optimize data... " + export_txt_append)
					
			for i = 1 to vertices_all.count do
			(
				cur_vertex = vertices_all[i]
				
				j = 1
				
				while j <= vertices.count do
				(
					wrk_vertex = vertices[j]
					
					if 	cur_vertex.coord.x ==wrk_vertex.coord.x and cur_vertex.coord.y == wrk_vertex.coord.y and cur_vertex.coord.z == wrk_vertex.coord.z and 
						cur_vertex.normal.x == wrk_vertex.normal.x and cur_vertex.normal.y == wrk_vertex.normal.y and cur_vertex.normal.z == wrk_vertex.normal.z and
						( (texture_presented==0) or (cur_vertex.texcoord.x == wrk_vertex.texcoord.x and cur_vertex.texcoord.y == wrk_vertex.texcoord.y) ) then exit
							
					j += 1
				)
				
				if j == vertices.count+1 then
					append vertices cur_vertex

				append indexes j

				progress+=1
				if not progressUpdate (100.*progress/max_progress) then return mesh_info 0 0 0
			)
		)
		else
		(
			progressStart ("Building buffers data... " + export_txt_append)
			for i = 1 to vertices_all.count do
			(
				append vertices vertices_all[i]
				append indexes i
				progress+=1
				if not progressUpdate (100.*progress/max_progress) then return mesh_info 0 0 0
			)
		)
		print ("Done. " + vertices.count as string + " vertices")
	
		print "Writing to file..."
		progressStart ("Writing to file... " + export_txt_append)

		writefloat	file meshObj.center.x
		writefloat	file meshObj.center.y
		writefloat	file meshObj.center.z

		bbox = nodeLocalBoundingBox meshObj

		writefloat	file (abs(bbox[2].x - bbox[1].x)/2)
		writefloat	file (abs(bbox[2].y - bbox[1].y)/2)
		writefloat	file (abs(bbox[2].z - bbox[1].z)/2)	
		
		for i = 1 to vertices.count do
		(
			writefloat file vertices[i].coord.x
			writefloat file vertices[i].coord.y
			writefloat file vertices[i].coord.z				
		)
		if not progressUpdate 25 then return mesh_info 0 0 0

		for i = 1 to vertices.count do
		(
			writefloat file vertices[i].normal.x
			writefloat file vertices[i].normal.y
			writefloat file vertices[i].normal.z		
		)
		if not progressUpdate 50 then return mesh_info 0 0 0

		if texture_presented!=0 then
			for i = 1 to vertices.count do
			(
				writefloat file vertices[i].texcoord.x
				writefloat file vertices[i].texcoord.y				
			)
		if not progressUpdate 75 then return mesh_info 0 0 0

		write_long = vertices.count > 65535
			
		for i = 1 to indexes.count do
		(
			if  write_long then
				writelong file (indexes[i] - 1)  #unsigned
			else		
				writeshort file (indexes[i] - 1)  #unsigned
		)

		if not progressUpdate 100 then return mesh_info 0 0 0
	
		fflush file
		print "Done."

		return mesh_info vertices.count (indexes.count/3) texture_presented
	)
	-------------------------------------------------------------------------------------
	function SaveMeshToFile FileName meshObj buildFNormals doCenter doScale=
	(
		disableSceneRedraw()
		
		if SuperClassOf meshObj != GeometryClass then
		(	
			print "Error: Node is not a valid geometry class."
			return false
		)
		
		if ClassOf meshObj != Editable_mesh then
		(
			if canConvertTo meshObj TriMeshGeometry and yesNoCancelBox ("Convert object \""+meshObj.name+"\" to editable mesh?") == #yes then
				convertToMesh meshObj
			else
			(	
				print "Error: Can't convert to editable mesh."
				return false
			)
		)
		
		print ("Exporting mesh:"+meshObj.name)	
	
		scaleK = 1.
		
		if doScale then
		(
			bbox = nodeLocalBoundingBox meshObj
			w = abs(bbox[2].x - bbox[1].x)
			h  = abs(bbox[2].y - bbox[1].y)
			d =  abs(bbox[2].z - bbox[1].z)
			m = w
			if w<h then m = h
			if m<d then m = d
			scaleK = 1./m
		)
		
		cenVec = [0., 0. , 0.]
		
		if(doCenter) then
			cenVec =  [-meshObj.center.x, -meshObj.center.y, -meshObj.center.z]

		file = fopen FileName "wb"
		if file == undefined then
		(		
			print "Couldn't open file for writing !"
			MessageBox "Couldn't open file for writing !"
			return false
		)
		else
		(	
			--Define flags
			flags = 0
			if do_frame_anim then flags = 1
				
			writestring 	file "DMD 3.0 BIN"
			fseek 		file 11 #seek_set
			writelong	file flags  #unsigned --ExtFlags	
			writebyte	file 0 #unsigned
			writelong	file 0 #unsigned
			writelong	file 0 #unsigned
		
			if flags == 1 then
				writelong	file 0 #unsigned
	
			if do_frame_anim then
			(
				cur_frame = start_frame
				while cur_frame <= end_frame do
				(
					slidertime = cur_frame
					minfo = ExportMesh file meshObj buildFNormals scaleK cenVec
					if (minfo.vcount + minfo.fcount + minfo.textured) == 0 then
					(
						fclose file
						progressEnd()
						enableSceneRedraw()
						return false
					)
					cur_frame += anim_step
				)
			)
			else
				minfo = ExportMesh file meshObj buildFNormals scaleK cenVec
			
			fseek 		file 15 #seek_set
			writebyte	file minfo.textured
			writelong	file minfo.vcount #unsigned 
			writelong	file minfo.fcount #unsigned 

			if flags == 1 then
				writelong	file ((end_frame - start_frame)/anim_step) #unsigned
			
			fclose file
			
			progressEnd()
		)

		print "Mesh export is done."
		enableSceneRedraw()
			
		return true
	)
	-------------------------------------------------------------------------------------
	rollout roll_sce "Scene Export"
	(
		label lexptodir "Exporting scene to directory."

		progressbar prog color:blue
		button btn_dir "Save As..." width:100
		
		on btn_dir pressed do
		(
		
	
		)

	)
	-------------------------------------------------------------------------------------
	function ExportSelectedNodes =
	(
		print "Model(s) exporting started..."

		skipped 		= 0
		i 				= 0
		export_txt_append = ""

		for node in selection do
		(
			export_txt_append = "(" + (i+1) as string + " of " + selection.count as string + ")"
			
			fname = GetSaveFileName types:"DMD Models Files (*.dmd)|*.dmd|All Files(*.*)|*.*|"
		
			if fname != undefined then
			(
				if not SaveMeshToFile fname node build_faset center_and_scale center_and_scale then skipped += 1
			)
			else
			(
				print "Error: Model filename is undefined."
				skipped += 1
			)
			
			i += 1						
		)
		
		print ("Model(s) exporting done. " + skipped as string + " model(s) of " + selection.count as string + " skipped")
	)
	-------------------------------------------------------------------------------------	
	rollout roll_sel_m "Selected Mesh"
	(
		label lexpmesh "Exporting currently selected mesh."
		
		checkbox cb_CenScModel "Centre and scale mesh"
		checkbox cb_BuildFN "Build faset normals"
		checkbox cb_GMOptimize "Optimize geometry (slow)"
	
		checkbox cb_DoAnimation "Export animation"
		
		radiobuttons rb_AnimType labels:#("Frame animation","Skeletal animation") default:1
		
		label lframes "Frames:"
		spinner sp_start	"From:" range:[animationrange.start,animationrange.end,animationrange.start] type:#Integer
		spinner sp_end	"To:" range:[animationrange.start,animationrange.end,animationrange.end] type:#Integer
       	spinner sp_step	"Step:" range:[1,animationrange.end,1] type:#Integer
		
		button btn_save "Save As..." width:100

		on roll_sel_m open do
		(
			cb_CenScModel.checked 	= true
			cb_BuildFN.checked 			= false
			sp_start.enabled 				= false
			sp_end.enabled 				= false
			sp_step.enabled 				= false
			lframes.enabled 				= false
			rb_AnimType.enabled 		= false
		)
		
		on cb_DoAnimation changed state do
		(
			if cb_DoAnimation.checked then
			(
			sp_start.enabled 			= true
			sp_end.enabled 			= true
			sp_step.enabled 			= true
			lframes.enabled 			= true
			rb_AnimType.enabled 	= true
			)
			else
			(
			sp_start.enabled 			= false
			sp_end.enabled 			= false
			sp_step.enabled 			= false		
			lframes.enabled 			= false
			rb_AnimType.enabled 	= false
			)
		)
		
		on btn_save pressed do
		(	
			
			if rb_AnimType.state == 2 then
				MessageBox "Sorry, not done yet."
			else
			(		
			center_and_scale =cb_CenScModel.checked
			build_faset = cb_BuildFN.checked
			gm_optimize = cb_GMOptimize.checked
			start_frame = sp_start.value
			end_frame = sp_end.value
			anim_step = sp_step.value
			do_frame_anim = ((rb_AnimType.state == 1) and cb_DoAnimation.checked)
			DestroyDialog roll_sel_m	
			ExportSelectedNodes()
			)				
		)
		
	)
	-------------------------------------------------------------------------------------
	on btn_export pressed do
	(
		case rb_ExScene.state of
		(
			1: MessageBox "Sorry, not done yet." --createDialog roll_sce		modal:true
			2: createDialog roll_sel_m  modal:true
		)
	)
	-------------------------------------------------------------------------------------
	rollout roll_about "About" width:200	
	(
		label abl_1 "DMD Models and Scenes Exporter."
		label abl_2 "V Beta 1"
		label abl_3 "For DGLE2."
		label abl_4 "(c)DRON 2010"	
		label abl_5 "http://dron.deeprosoft.com/dgle2"
		button btn_ok "Ok" width:50
		on btn_ok pressed do
 			DestroyDialog roll_about
	)
	on btn_about pressed do 
		createDialog roll_about	modal:true
	-------------------------------------------------------------------------------------
) 