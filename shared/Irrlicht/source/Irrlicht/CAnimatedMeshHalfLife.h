// Copyright (C) 2002-2010 Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ANIMATED_MESH_HALFLIFE_H_INCLUDED__
#define __C_ANIMATED_MESH_HALFLIFE_H_INCLUDED__

#include "IAnimatedMesh.h"
#include "ISceneManager.h"
#include "irrArray.h"
#include "irrString.h"
#include "IMeshLoader.h"
#include "SMesh.h"
#include "IReadFile.h"

namespace irr
{
namespace scene
{

	
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

	// STUDIO MODELS, Copyright (c) 1998, Valve LLC. All rights reserved.
	#define MAXSTUDIOTRIANGLES	20000	// TODO: tune this
	#define MAXSTUDIOVERTS		2048	// TODO: tune this
	#define MAXSTUDIOSEQUENCES	256		// total animation sequences
	#define MAXSTUDIOSKINS		100		// total textures
	#define MAXSTUDIOSRCBONES	512		// bones allowed at source movement
	#define MAXSTUDIOBONES		128		// total bones actually used
	#define MAXSTUDIOMODELS		32		// sub-models per model
	#define MAXSTUDIOBODYPARTS	32
	#define MAXSTUDIOGROUPS		4
	#define MAXSTUDIOANIMATIONS	512		// per sequence
	#define MAXSTUDIOMESHES		256
	#define MAXSTUDIOEVENTS		1024
	#define MAXSTUDIOPIVOTS		256
	#define MAXSTUDIOCONTROLLERS 8

	typedef f32 vec3_hl[3];	// x,y,z
	typedef f32 vec4_hl[4];	// x,y,z,w

	struct SHalflifeHeader
	{
		c8 id[4];
		s32 version;

		c8 name[64];
		s32 length;

		vec3_hl eyeposition;	// ideal eye position
		vec3_hl min;			// ideal movement hull size
		vec3_hl max;			

		vec3_hl bbmin;			// clipping bounding box
		vec3_hl bbmax;		

		s32	flags;

		u32	numbones;			// bones
		u32	boneindex;

		u32	numbonecontrollers;		// bone controllers
		u32	bonecontrollerindex;

		u32	numhitboxes;			// complex bounding boxes
		u32	hitboxindex;			

		u32	numseq;				// animation sequences
		u32	seqindex;

		u32	numseqgroups;		// demand loaded sequences
		u32	seqgroupindex;

		u32	numtextures;		// raw textures
		u32	textureindex;
		u32	texturedataindex;

		u32	numskinref;			// replaceable textures
		u32	numskinfamilies;
		u32	skinindex;

		u32	numbodyparts;		
		u32	bodypartindex;

		u32	numattachments;		// queryable attachable points
		u32	attachmentindex;

		s32	soundtable;
		s32	soundindex;
		s32	soundgroups;
		s32	soundgroupindex;

		s32 numtransitions;		// animation node to animation node transition graph
		s32	transitionindex;
	};

	// header for demand loaded sequence group data
	typedef struct 
	{
		s32 id;
		s32 version;

		c8 name[64];
		s32 length;
	} studioseqhdr_t;

	// bones
	struct SHalflifeBone
	{
		c8 name[32];	// bone name for symbolic links
		s32 parent;		// parent bone
		s32 flags;		// ??
		s32 bonecontroller[6];	// bone controller index, -1 == none
		f32 value[6];	// default DoF values
		f32 scale[6];   // scale for delta DoF values
	};


	// bone controllers
	struct SHalflifeBoneController
	{
		s32 bone;	// -1 == 0
		s32 type;	// X, Y, Z, XR, YR, ZR, M
		f32 start;
		f32 end;
		s32 rest;	// byte index value at rest
		s32 index;	// 0-3 user set controller, 4 mouth
	};

	// intersection boxes
	struct SHalflifeBBox
	{
		s32 bone;
		s32 group;			// intersection group
		vec3_hl bbmin;		// bounding box
		vec3_hl bbmax;		
	};

#ifndef ZONE_H
	typedef void *cache_user_t;
#endif

	// demand loaded sequence groups
	struct SHalflifeSequenceGroup
	{
		c8 label[32];	// textual name
		c8 name[64];	// file name
		cache_user_t cache;		// cache index pointer
		s32 data;		// hack for group 0
	};

	// sequence descriptions
	struct SHalflifeSequence
	{
		c8 label[32];	// sequence label

		f32 fps;		// frames per second	
		s32 flags;		// looping/non-looping flags

		s32 activity;
		s32 actweight;

		s32 numevents;
		s32 eventindex;

		s32 numframes;	// number of frames per sequence

		u32 numpivots;	// number of foot pivots
		u32 pivotindex;

		s32 motiontype;	
		s32 motionbone;
		vec3_hl linearmovement;
		s32 automoveposindex;
		s32 automoveangleindex;

		vec3_hl bbmin;		// per sequence bounding box
		vec3_hl bbmax;		

		s32 numblends;
		s32 animindex;		// SHalflifeAnimOffset pointer relative to start of sequence group data
		// [blend][bone][X, Y, Z, XR, YR, ZR]

		s32 blendtype[2];	// X, Y, Z, XR, YR, ZR
		f32 blendstart[2];	// starting value
		f32 blendend[2];	// ending value
		s32 blendparent;

		s32 seqgroup;		// sequence group for demand loading

		s32 entrynode;		// transition node at entry
		s32 exitnode;		// transition node at exit
		s32 nodeflags;		// transition rules

		s32 nextseq;		// auto advancing sequences
	};

	// events
	typedef struct 
	{
		s32 frame;
		s32 event;
		s32 type;
		c8 options[64];
	} mstudioevent_t;


	// pivots
	typedef struct 
	{
		vec3_hl org;	// pivot point
		s32 start;
		s32 end;
	} mstudiopivot_t;

	// attachment
	struct SHalfelifeAttachment
	{
		c8 name[32];
		s32 type;
		s32 bone;
		vec3_hl org;	// attachment point
		vec3_hl vectors[3];
	};

	struct SHalflifeAnimOffset
	{
		u16	offset[6];
	};

	// animation frames
	union SHalfelifeAnimationFrame
	{
		struct {
			u8	valid;
			u8	total;
		} num;
		s16		value;
	};


	// body part index
	struct SHalflifeBody
	{
		c8 name[64];
		u32 nummodels;
		u32 base;
		u32 modelindex; // index into models array
	};


	// skin info
	struct SHalflifeTexture
	{
		c8 name[64];
		s32 flags;
		s32 width;
		s32 height;
		s32 index;
	};


	// skin families
	// short	index[skinfamilies][skinref]

	// studio models
	struct SHalflifeModel
	{
		c8 name[64];
		s32 type;

		f32	boundingradius;

		u32	nummesh;
		u32	meshindex;

		u32	numverts;		// number of unique vertices
		u32	vertinfoindex;	// vertex bone info
		u32	vertindex;		// vertex vec3_hl
		u32	numnorms;		// number of unique surface normals
		u32	norminfoindex;	// normal bone info
		u32	normindex;		// normal vec3_hl

		u32	numgroups;		// deformation groups
		u32	groupindex;
	};


	// meshes
	typedef struct 
	{
		u32	numtris;
		u32	triindex;
		u32	skinref;
		u32	numnorms;		// per mesh normals
		u32	normindex;		// normal vec3_hl
	} SHalflifeMesh;

			// lighting options
	#define STUDIO_NF_FLATSHADE		0x0001
	#define STUDIO_NF_CHROME		0x0002
	#define STUDIO_NF_FULLBRIGHT	0x0004

			// motion flags
	#define STUDIO_X		0x0001
	#define STUDIO_Y		0x0002	
	#define STUDIO_Z		0x0004
	#define STUDIO_XR		0x0008
	#define STUDIO_YR		0x0010
	#define STUDIO_ZR		0x0020
	#define STUDIO_LX		0x0040
	#define STUDIO_LY		0x0080
	#define STUDIO_LZ		0x0100
	#define STUDIO_AX		0x0200
	#define STUDIO_AY		0x0400
	#define STUDIO_AZ		0x0800
	#define STUDIO_AXR		0x1000
	#define STUDIO_AYR		0x2000
	#define STUDIO_AZR		0x4000
	#define STUDIO_TYPES	0x7FFF
	#define STUDIO_RLOOP	0x8000	// controller that wraps shortest distance

			// sequence flags
	#define STUDIO_LOOPING	0x0001

			// bone flags
	#define STUDIO_HAS_NORMALS	0x0001
	#define STUDIO_HAS_VERTICES 0x0002
	#define STUDIO_HAS_BBOX		0x0004
	#define STUDIO_HAS_CHROME	0x0008	// if any of the textures have chrome on them

	#define RAD_TO_STUDIO		(32768.0/M_PI)
	#define STUDIO_TO_RAD		(M_PI/32768.0)

		// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( pop, packing )
#endif
#undef PACK_STRUCT

	/*!
		Textureatlas
		Combine Source Images with arbitrary size and bithdepth to an Image with 2^n size
		borders from the source images are copied around for allowing filtering ( bilinear, mipmap )
	*/
	struct STextureAtlas
	{
		STextureAtlas ()
		{
			release();
		}

		virtual ~STextureAtlas ()
		{
			release ();
		}

		void release ();
		void addSource ( const c8 * name, video::IImage * image );
		void create ( u32 pixelborder, video::E_TEXTURE_CLAMP texmode );
		void getScale ( core::vector2df &scale );
		void getTranslation ( const c8 * name, core::vector2di &pos );

		struct TextureAtlasEntry
		{
			io::path name;
			u32 width;
			u32 height;

			core::vector2di pos;

			video::IImage * image;

			bool operator < ( const TextureAtlasEntry & other )
			{
				return height > other.height;
			}
		};


		core::array < TextureAtlasEntry > atlas;
		video::IImage * Master;
	};


	class CAnimatedMeshHalfLife : public IAnimatedMesh
	{
	public:

		//! constructor
		CAnimatedMeshHalfLife( );

		//! destructor
		virtual ~CAnimatedMeshHalfLife();

		//! loads a Halflife mdl file
		virtual bool loadModelFile( io::IReadFile* file, ISceneManager * smgr );

		//IAnimatedMesh
		virtual u32 getFrameCount() const;
		virtual IMesh* getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop);
		virtual const core::aabbox3d<f32>& getBoundingBox() const;
		virtual E_ANIMATED_MESH_TYPE getMeshType() const;
		virtual void renderModel ( u32 param, video::IVideoDriver * driver, const core::matrix4 &absoluteTransformation);

		//! returns amount of mesh buffers.
		virtual u32 getMeshBufferCount() const;
		//! returns pointer to a mesh buffer
		virtual IMeshBuffer* getMeshBuffer(u32 nr) const;
		//! Returns pointer to a mesh buffer which fits a material
		virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const;

		virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX);

		//! flags the meshbuffer as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX);

		//! set user axis aligned bounding box
		virtual void setBoundingBox(const core::aabbox3df& box);

		//! Get the Animation List
		virtual IAnimationList* getAnimList () { return &AnimList; }

		//! Return the named Body List of this Animated Mesh
		virtual IBodyList *getBodyList() { return &BodyList; }

	private:

		// KeyFrame Animation List
		IAnimationList AnimList;
		// Sum of all sequences
		u32 FrameCount;

		// Named meshes of the Body
		IBodyList BodyList;

		//! return a Mesh per frame
		SMesh MeshIPol;


		ISceneManager *SceneManager;

		SHalflifeHeader *Header;
		SHalflifeHeader *TextureHeader;
		bool OwnTexModel;						// do we have a modelT.mdl ?
		SHalflifeHeader *AnimationHeader[32];	// sequences named model01.mdl, model02.mdl

		void initData ();
		void freeModel ();
		SHalflifeHeader * loadModel( io::IReadFile* file, const io::path &filename );
		bool postLoadModel( const io::path &filename );

		u32 SequenceIndex;	// sequence index
		f32 CurrentFrame;	// Current Frame

		#define MOUTH_CONTROLLER	4
		u8  BoneController[4 + 1 ];	// bone controllers + mouth position
		u8	Blending[2];		// animation blending

		f32 SetController( s32 controllerIndex, f32 value );


		u32	SkinGroupSelection;			// skin group selection
		u32 SetSkin( u32 value );

		void initModel ();
		void dumpModelInfo ( u32 level);

		void ExtractBbox( s32 sequence, core::aabbox3df &box );


		void setUpBones ();
		SHalflifeAnimOffset * getAnim( SHalflifeSequence *seq );
		void slerpBones( vec4_hl q1[], vec3_hl pos1[], vec4_hl q2[], vec3_hl pos2[], f32 s );
		void calcRotations ( vec3_hl *pos, vec4_hl *q, SHalflifeSequence *seq, SHalflifeAnimOffset *anim, f32 f );

		vec4_hl BoneAdj;
		void calcBoneAdj();
		void calcBoneQuaternion( s32 frame, f32 s, SHalflifeBone *bone, SHalflifeAnimOffset *anim, f32 *q ) const;
		void calcBonePosition( s32 frame, f32 s, SHalflifeBone *bone, SHalflifeAnimOffset *anim, f32 *pos ) const;

		void buildVertices ();

		io::path TextureBaseName;

#define HL_TEXTURE_ATLAS


#ifdef HL_TEXTURE_ATLAS
		STextureAtlas TextureAtlas;
		video::ITexture *TextureMaster;
#endif
		
	};


	//! Meshloader capable of loading HalfLife Model files
	class CHalflifeMDLMeshFileLoader : public IMeshLoader
	{
	public:

		//! Constructor
		CHalflifeMDLMeshFileLoader( scene::ISceneManager* smgr );

		//! returns true if the file maybe is able to be loaded by this class
		//! based on the file extension (e.g. ".bsp")
		virtual bool isALoadableFileExtension(const io::path& filename) const;

		//! creates/loads an animated mesh from the file.
		//! \return Pointer to the created mesh. Returns 0 if loading failed.
		//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
		//! See IReferenceCounted::drop() for more information.
		virtual IAnimatedMesh* createMesh(io::IReadFile* file);

	private:
		scene::ISceneManager* SceneManager;
	};


} // end namespace scene
} // end namespace irr

#endif

