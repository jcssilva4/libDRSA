
# ======================================================
# 		UNIVERSIDADE FEDERAL DE PERNANMBUCO
#		CENTRO DE INFORMATICA (CIn-UFPE)
#Author: 	Julio Silva
#Contact:	jcss4@cin.ufpe.br
# ======================================================


CXXFLAGS = -g -Wall

APP_DIR = $(HOME)/applications

#DRSAlib directory
PROJ_DIR = /home/juliocss/Desktop/DRSA

CXX = g++

INCLUDES = -I$(PROJ_DIR)/include  

OBJ_DIR = $(PROJ_DIR)/objs
SRC_DIR = $(PROJ_DIR)/src

OBJECTS_MACHINE_KNOWLEDGE = $(OBJ_DIR)/Obj.o 
OBJECTS_METHOD = $(OBJ_DIR)/Approximation.o $(OBJ_DIR)/DRSA.o
OBJECTS_RULE_INDUCTION = $(OBJ_DIR)/Domlem.o $(OBJ_DIR)/Rule.o $(OBJ_DIR)/RuleInductionAlgorithm.o
OBJECTS= $(OBJECTS_MACHINE_KNOWLEDGE) $(OBJECTS_METHOD) $(OBJECTS_RULE_INDUCTION) 

STATICLIB=$(PROJ_DIR)/lib/libdrsa.a

all:$(STATICLIB)

#chkopts

$(STATICLIB):	$(OBJECTS)
	@echo 'Linking....'
	ar crs $(STATICLIB) $(OBJECTS) 
	@echo "ok. Done!"

#Machine Knowledge
$(OBJ_DIR)/%.o:	$(SRC_DIR)/machineKnowledge/%.cpp 
	$(CXX) $(CXXFLAGS) -c -fPIC $(INCLUDES)  $< #you need to use -fPIC, otherwise 'setup.py build' will tell you to do so
	@mv *.o $(OBJ_DIR)

#Method
$(OBJ_DIR)/%.o: $(SRC_DIR)/method/%.cpp 
	$(CXX) $(CXXFLAGS) -c -fPIC $(INCLUDES)  $< #you need to use -fPIC, otherwise 'setup.py build' will tell you to do so
	@mv *.o $(OBJ_DIR)

#Rule Induction
$(OBJ_DIR)/%.o: $(SRC_DIR)/rule_induction/%.cpp 
	$(CXX) $(CXXFLAGS) -c -fPIC $(INCLUDES)  $< #you need to use -fPIC, otherwise 'setup.py build' will tell you to do so
	@mv *.o $(OBJ_DIR)

cleanup:
	rm  $(OBJ_DIR)/*.o
	rm  $(PROJ_DIR)/lib/*.a
