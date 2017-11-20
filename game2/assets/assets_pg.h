#define IDR_DOORS					 	 					0x1000
#define IDR_DOORS_OPEN					 	 			0x00 + IDR_DOORS
#define IDR_DOORS_CLOSE					 	 			0x10 + IDR_DOORS
#define IDR_DOORS_UP					 	 				0x20 + IDR_DOORS
#define IDR_DOORS_DOWN					 	 			0x30 + IDR_DOORS

#define IDR_CALIB								 	 		0x2000

#define IDR_CALIB_ITEMS				 	 				0x00
#define IDR_CALIB_ITEM_OFF	 	 			 		0x10
#define IDR_CALIB_ITEM_RESULT	 	 				0x20

#define IDR_CALIB_LEVEL 				 	 		 0x000 + IDR_CALIB

#define IDR_CALIB_LEVEL1				 	 		 0x100 + IDR_CALIB_LEVEL
#define IDR_CALIB_LEVEL1_ITEMS								 IDR_CALIB_LEVEL1 +	IDR_CALIB_ITEMS
#define IDR_CALIB_LEVEL1_ITEM_OFF	 	 					 IDR_CALIB_LEVEL1 + IDR_CALIB_ITEM_OFF
#define IDR_CALIB_LEVEL1_ITEM_RESULT 	 				 IDR_CALIB_LEVEL1 + IDR_CALIB_ITEM_RESULT

#define IDR_CALIB_LEVEL2				 	 		 0x200 + IDR_CALIB_LEVEL
#define IDR_CALIB_LEVEL2_ITEMS								 IDR_CALIB_LEVEL2 +	IDR_CALIB_ITEMS
#define IDR_CALIB_LEVEL2_ITEM_OFF	 	 					 IDR_CALIB_LEVEL2 + IDR_CALIB_ITEM_OFF
#define IDR_CALIB_LEVEL2_ITEM_RESULT 	 				 IDR_CALIB_LEVEL2 + IDR_CALIB_ITEM_RESULT

#define IDR_AVATAR					    			0x3000
#define IDR_AVATAR_BODY					  			0x00 + IDR_AVATAR
#define IDR_AVATAR_EYES									0x10 + IDR_AVATAR
#define IDR_AVATAR_HAIR									0x20 + IDR_AVATAR
#define IDR_AVATAR_SHIRT								0x30 + IDR_AVATAR

#define IDR_PROFILER					    		0x4000
#define IDR_PROFILER_BACKGROUND	  			0x00 + IDR_PROFILER
#define IDR_PROFILER_BUTTONS	  				0x10 + IDR_PROFILER
#define IDR_PROFILER_SELECTION	  			0x20 + IDR_PROFILER
#define IDR_PROFILER_TITLE	  					0x30 + IDR_PROFILER

#define IDR_INSTRUCTIONS							0x5000
#define IDR_INSTRUCTIONS_SITTING				0x00 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_START					0x10 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_CALIBRATE			0x20 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_LOGIN					0x30 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_REWARDS		    0x40 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_REWARDS_BONUS     1 + IDR_INSTRUCTIONS_REWARDS
#define IDR_INSTRUCTIONS_POSITION_ALERT	0x50 + IDR_INSTRUCTIONS

#define IDR_INSTRUCTIONS_LEVEL1				 		 0x100 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_LEVEL1_ITEM	 			0x00 + IDR_INSTRUCTIONS_LEVEL1
#define IDR_INSTRUCTIONS_LEVEL1_ITEM_RED			 0 + IDR_INSTRUCTIONS_LEVEL1_ITEM
#define IDR_INSTRUCTIONS_LEVEL1_ITEM_GREEN		 1 + IDR_INSTRUCTIONS_LEVEL1_ITEM
#define IDR_INSTRUCTIONS_LEVEL1_ITEM_BLUE			 2 + IDR_INSTRUCTIONS_LEVEL1_ITEM
#define IDR_INSTRUCTIONS_LEVEL1_ITEM_YELLOW		 3 + IDR_INSTRUCTIONS_LEVEL1_ITEM
#define IDR_INSTRUCTIONS_LEVEL1_RESULT 			0x10 + IDR_INSTRUCTIONS_LEVEL1

#define IDR_INSTRUCTIONS_LEVEL2				 		 0x200 + IDR_INSTRUCTIONS
#define IDR_INSTRUCTIONS_LEVEL2_ITEM	 			0x00 + IDR_INSTRUCTIONS_LEVEL2
#define IDR_INSTRUCTIONS_LEVEL2_ITEM_CIRCLE		 0 + IDR_INSTRUCTIONS_LEVEL2_ITEM
#define IDR_INSTRUCTIONS_LEVEL2_ITEM_SQUARE		 1 + IDR_INSTRUCTIONS_LEVEL2_ITEM
#define IDR_INSTRUCTIONS_LEVEL2_ITEM_STAR			 2 + IDR_INSTRUCTIONS_LEVEL2_ITEM
#define IDR_INSTRUCTIONS_LEVEL2_ITEM_TRIANGLE	 3 + IDR_INSTRUCTIONS_LEVEL2_ITEM
#define IDR_INSTRUCTIONS_LEVEL2_RESULT 			0x10 + IDR_INSTRUCTIONS_LEVEL2

#define IDR_STORY					 	 					0x6000

#define IDR_STORY_LEVEL1	 	 					 0x100 + IDR_STORY
#define IDR_STORY_LEVEL1_BACKGROUND			0x10 + IDR_STORY_LEVEL1
#define IDR_STORY_LEVEL1_ALERT					0x20 + IDR_STORY_LEVEL1

#define IDR_STORY_LEVEL2	 	 					 0x200 + IDR_STORY
#define IDR_STORY_LEVEL2_BOX_CLOSED			0x10 + IDR_STORY_LEVEL2
#define IDR_STORY_LEVEL2_BOX_OPENED			0x20 + IDR_STORY_LEVEL2
#define IDR_STORY_LEVEL2_BUTTON_GREEN		0x30 + IDR_STORY_LEVEL2
#define IDR_STORY_LEVEL2_BUTTON_RED			0x40 + IDR_STORY_LEVEL2
#define IDR_STORY_LEVEL2_GHOSTS_ESCAPE	0x50 + IDR_STORY_LEVEL2
#define IDR_STORY_LEVEL2_GHOSTS_TRAPPED	0x60 + IDR_STORY_LEVEL2


#define IDR_STATUS					    			0x7000
#define IDR_STATUS_BACKGROUND	  				0x00 + IDR_STATUS
#define IDR_STATUS_PANEL	  						0x10 + IDR_STATUS
#define IDR_STATUS_BUTTON_CONTINUE			0x20 + IDR_STATUS

#define IDR_PRIZE 	       						0x7100
#define IDR_PRIZE_ICON   						 	 0x100 + IDR_PRIZE
#define IDR_PRIZE_CENTER   						 0x200 + IDR_PRIZE

#define IDR_PRIZE_BEAR_BROWN		 				0x00
#define IDR_PRIZE_BEAR_PURPLE		 				0x01
#define IDR_PRIZE_CAT_BROWN		 					0x02
#define IDR_PRIZE_CAT_GREY		 					0x03
#define IDR_PRIZE_LIGHTNING_RED					0x04
#define IDR_PRIZE_LIGHTNING_TURQ				0x05
#define IDR_PRIZE_MASK_GREEN						0x06
#define IDR_PRIZE_MASK_PURPLE						0x07
#define IDR_PRIZE_RABBIT								0x08
#define IDR_PRIZE_SHERIFF								0x09

