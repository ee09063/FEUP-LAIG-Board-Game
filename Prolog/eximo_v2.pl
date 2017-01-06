%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% MODULES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
:-use_module(library(lists)).
:-use_module(library(random)).
:-use_module(library(sockets)).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% SOCKETS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

port(60070).

% launch me in sockets mode
server:-
	port(Port),
	socket_server_open(Port, Socket),
	socket_server_accept(Socket, _Client, Stream, [type(text)]),
	write('Accepted connection'), nl,
	serverLoop(Stream),
	socket_server_close(Socket).
	
% wait for commands
serverLoop(Stream) :-
	repeat,
	read(Stream, ClientMsg),
	write('Received: '), write(ClientMsg), nl,
	parse_input(ClientMsg, MyReply),
	format(Stream, '~q.~n', [MyReply]),
	write('Wrote: '), write(MyReply), nl,
	flush_output(Stream),
	(ClientMsg == quit; ClientMsg == end_of_file), !.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%% COMMAND PARSING %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
parse_input(quit, ok-bye) :- !.
	
parse_input(inputStartPosition(Player, Board, Row-Col), Answer):-
	inputStartPosition(Player, Board, Row-Col, Answer).
	
parse_input(inputDestination(Player, Board, PRow-PCol, TRow-TCol, PlayType), Answer):-
	inputDestination(Player, Board, PRow-PCol, TRow-TCol, PlayType, Answer).
	
parse_input(mustSavePiece(Player, Board-CapW-CapB), Answer):-
	mustSavePiece(Player, Board-CapW-CapB, Answer).
	
parse_input(mustMoveAgain(Player, Board, PlayType, Row-Col), Answer):-
	mustMoveAgain(Player, Board, PlayType, Row-Col, Answer).

parse_input(possibleMove(Player, Board, Row-Col), Answer):-
	possibleMove(Player, Board, Row-Col, Answer).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% CUSTOM MADE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inputStartPosition(Player, Board, Row-Col, Answer):-
	getStartPositions(Player, Board, StartPositions),
	checkBelong(Row-Col, StartPositions, Answer).
	
checkBelong(Row-Col, StartPositions, Answer):-
	member(Row-Col, StartPositions),
	Answer = 1.

checkBelong(Row-Col, StartPositions, Answer):-
	\+member(Row-Col, StartPositions),
	Answer = -1.
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inputDestination(Player, Board, PRow-PCol, TRow-TCol, PlayType, Answer):-
	PlayType == 0,
	getAllMovePositions(Player, Board, PRow-PCol, MovePositions),
	getAllJumpPositions(Player, Board, PRow-PCol, JumpPositions),
	getAllCapturePositions(Player, Board, PRow-PCol, CapturePositions),
	checkBelong2(Player, Board, TRow-TCol, MovePositions, JumpPositions, CapturePositions, Answer).

inputDestination(Player, Board, _PRow-_PCol, TRow-TCol, PlayType, Answer):-
	PlayType == 6,
	inputDestination2(Player, Board, TRow-TCol, Answer).
		
inputDestination(Player, Board, PRow-PCol, TRow-TCol, PlayType, Answer):-
	PlayType == 8,
	getAllJumpPositions(Player, Board, PRow-PCol, JumpPositions),
	checkBelong2(Player, Board, TRow-TCol, [], JumpPositions, [], Answer).
	
inputDestination(Player, Board, PRow-PCol, TRow-TCol, PlayType, Answer):-
	PlayType == 9,
	getAllCapturePositions(Player, Board, PRow-PCol, CapturePositions),
	checkBelong2(Player, Board, TRow-TCol, [], [], CapturePositions, Answer).
	
inputDestination2(Player, Board-_A-_B, TRow-TCol, Answer):-
	getAllFreePositions(Player, Board, FreePositions),
	checkBelong3(Player, Board, TRow-TCol, FreePositions, Answer).
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
checkBelong2(_Player, _Board, TRow-TCol, MovePositions, _JumpPositions, _CapturePositions, Answer):-
	member(TRow-TCol, MovePositions),
	Answer = 7.
	
checkBelong2(_Player, _Board, TRow-TCol, _MovePositions, JumpPositions, _CapturePositions, Answer):-
	member(TRow-TCol, JumpPositions),
	Answer = 8.
	
checkBelong2(_Player, _Board, TRow-TCol, _MovePositions, _JumpPositions, CapturePositions, Answer):-
	member(TRow-TCol, CapturePositions),
	Answer = 9.

checkBelong2(_Player, _Board, TRow-TCol, MovePositions, JumpPositions, CapturePositions, Answer):-
	\+member(TRow-TCol, MovePositions),
	\+member(TRow-TCol, JumpPositions),
	\+member(TRow-TCol, CapturePositions),
	Answer = -1.
	
checkBelong3(_Player, _Board, TRow-TCol, FreePositions, Answer):-
	member(TRow-TCol, FreePositions),
	Answer = 6.
	
checkBelong3(_Player, _Board, TRow-TCol, FreePositions, Answer):-
	\+member(TRow-TCol, FreePositions),
	Answer = -1.
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mustSavePiece(Player, Board-_CapByW-CapByB, Answer):-
	Player,
	CapByB > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, L),
	L > 0,
	Answer = 1.

mustSavePiece(Player, Board-_CapByW-CapByB, Answer):-
	Player,
	CapByB > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, L),
	L == 0,
	Answer = -1.

mustSavePiece(Player, _Board-_CapByW-CapByB, Answer):-
	Player,
	CapByB == 0,
	Answer = -1.
	
mustSavePiece(Player, Board-CapByW-_CapByB, Answer):-
	\+Player,
	CapByW > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, L),
	L > 0,
	Answer = 1.	

mustSavePiece(Player, Board-CapByW-_CapByB, Answer):-
	\+Player,
	CapByW > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, L),
	L == 0,
	Answer = -1.

mustSavePiece(Player, _Board-CapByW-_CapByB, Answer):-
	\+Player,
	CapByW == 0,
	Answer = -1.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mustMoveAgain(Player, Board, PlayType, Row-Col, Answer):-	
	PlayType == 8,
	getAllJumpPositions(Player, Board, Row-Col, JumpPositions),
	length(JumpPositions, L),
	L > 0,
	Answer = 1.

mustMoveAgain(Player, Board, PlayType, Row-Col, Answer):-	
	PlayType == 9,
	getAllCapturePositions(Player, Board, Row-Col, CapturePositions),
	write(CapturePositions),nl,
	length(CapturePositions, L),
	L > 0,
	Answer = 1.
	
mustMoveAgain(Player, Board, PlayType, Row-Col, Answer):-	
	PlayType == 8,
	getAllJumpPositions(Player, Board, Row-Col, JumpPositions),
	length(JumpPositions, L),
	L == 0,
	Answer = -1.

mustMoveAgain(Player, Board, PlayType, Row-Col, Answer):-	
	PlayType == 9,
	getAllCapturePositions(Player, Board, Row-Col, CapturePositions),
	length(CapturePositions, L),
	L == 0,
	Answer = -1.
	
possibleMove(Player, Board, Row-Col, Answer):-
	getAllMovePositions(Player, Board, Row-Col, MovePositions),
	getAllJumpPositions(Player, Board, Row-Col, JumpPositions),
	getAllCapturePositions(Player, Board, Row-Col, CapturePositions),
	append(MovePositions, [], FP2),
	append(JumpPositions, FP2, FP3),
	append(CapturePositions, FP3, FP4),
	length(FP4, L),
	L > 0, 
	Answer = 1.

possibleMove(Player, Board, Row-Col, Answer):-
	getAllMovePositions(Player, Board, Row-Col, MovePositions),
	getAllJumpPositions(Player, Board, Row-Col, JumpPositions),
	getAllCapturePositions(Player, Board, Row-Col, CapturePositions),
	append(MovePositions, [], FP2),
	append(JumpPositions, FP2, FP3),
	append(CapturePositions, FP3, FP4),
	length(FP4, L),
	L = 0, 
	Answer = -1.	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% BOARD INITIALIZATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
initializeBoard([[0,1,1,1,1,1,1,0],
			     [0,1,1,1,1,1,1,0],
			     [0,1,1,0,0,1,1,0],
			     [0,0,0,0,0,0,0,0],
			     [0,0,0,0,0,0,0,0],
		         [0,2,2,0,0,2,2,0],
			     [0,2,2,2,2,2,2,0],
			     [0,2,2,2,2,2,2,0]]-0-0).
initializeBoardCapture(
				[[0,1,1,1,1,1,1,0],
				 [0,1,1,1,1,1,1,0],
			     [0,0,0,0,0,0,0,0],
			     [0,1,1,1,0,1,0,0],
			     [0,0,2,0,1,1,1,0],
		         [0,0,0,0,1,0,0,0],
			     [0,0,0,0,1,1,1,0],
			     [1,0,0,0,0,0,0,0]]-0-2).
initializeBoardJump(
				[[0,0,0,0,0,0,0,0],
				 [0,1,0,0,0,0,0,0],
			     [0,0,0,0,0,0,0,0],
			     [0,0,0,1,0,0,0,0],
			     [0,1,0,1,1,1,1,0],
		         [0,0,1,1,1,1,1,1],
			     [0,0,0,2,0,0,2,0],
			     [0,0,0,0,0,0,0,0]]-0-0).
initializeBoardCC(
				[[0,1,1,1,0,0,0,0],
				 [0,1,1,1,1,2,1,0],
			     [0,1,2,0,0,0,0,0],
			     [0,0,0,0,1,2,1,0],
			     [0,1,2,0,0,2,0,0],
		         [0,0,1,0,0,0,1,0],
			     [0,1,0,2,0,0,0,0],
			     [0,0,0,0,0,0,0,0]]-2-2).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% PRINTING FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MAIN PRINTING FUNCTION
printBoard(Board-CapByW-CapByB) :- nl, printCaptured(CapByW), nl, nl,
								   printHeader, nl,
								   printlists(Board,0),
								   printCaptured(CapByB), nl.
% PRINTS THE CAPTURED PIECES
printCaptured(Number) :- write('Captured Pieces: '), write(Number). 
	
% PRINTS THE BOARD
printlists([],_Count):- printLine, nl.
printlists([FirstList|OtherLists],Count) :- printLine, nl,
									        write(Count), Sum is Count+1,
									        printlist(FirstList),
									        printlists(OtherLists, Sum).	
% PRINTS A LINE OF THE BOARD
printlist([]) :- write('|'), nl.
printlist([FirstElem|OtherElems]) :- write('|'),
									 printElem(FirstElem),
									 printlist(OtherElems).
				 
% PRINTS THE HEADER OF THE BOARD											
printLine :- write('------------------').
printHeader :- write(' |0|1|2|3|4|5|6|7|').

% PRINT THE TURN OF THE PLAYER
printPlayerTurn(Player) :- Player, !, nl, write('White Turn'), nl.
printPlayerTurn(_Player) :- nl, write('Black Turn'), nl.

% PRINT ELEMENTS OF THE BOARD
printElem(Piece) :- Piece=0, write(' ').
printElem(Piece) :- Piece=1, write('w').
printElem(Piece) :- Piece=2, write('b').
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% PLAYER FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INITIALIZE PLAYER TO TRUE - WHITE
initializePlayer(Player) :- Player = true.

% CHANGE PLAYERS
nextTurn(true, false).
nextTurn(false, true).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%% START THE GAME %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
start :-
	initializeBoard(Board),
	start2(Board).

startCapture :- 
	initializeBoardCapture(Board),
	start2(Board).
				
startJump :-
	initializeBoardJump(Board),
	start2(Board).
	
startCC :-
	initializeBoardCC(Board),
	start2(Board).

start2(Board):-
	initializePlayer(Player),
	write('Select Game Mode and Difficulty'), nl,
	write('1 - H/H 2 - H/C 3- C/C'),nl,
	read(GameType),
	start3(Player, Board, GameType).
	
start3(Player, Board, GameType):-
	GameType = 1,
	run(Board, Player, GameType-1), nl.

start3(Player, Board, GameType):-
	GameType \= 1,
	write('Difficulty:'),nl,
	read(Dif),
	run(Board, Player, GameType-Dif), nl.	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%% MAIN PROGRAM CYCLE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
run(Board, Player, GameType-Dif) :- 			
	 
	 canStillPlay(Board, Player),
	 
	 printBoard(Board),
	 
	 printPlayerTurn(Player),
	 
	 processStartPositions(Player, Board, _StartPositions, StartRow-StartCol, GameType),
	 
	 processMovePositions(Player, Board, StartRow-StartCol, MovePositions),
	 processJumpPositions(Player, Board, StartRow-StartCol, JumpPositions),
	 processCapturePositions(Player, Board, StartRow-StartCol, CapturePositions),
	 
	 readPlay(Player, GameType-Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions),
	 
	 executePlay(Player, GameType, PlayType, Board, StartRow-StartCol, EndRow-EndCol, NewBoard),
	 
	 nextTurn(Player, NewPlayer), nl,
	 
	 run(NewBoard, NewPlayer, GameType-Dif).
					 
% WHITE PLAYER WINS
run(Board, Player, _GameType-_Dif) :-
								Player,
								printBoard(Board),
								write('Black WON').

% BLACK PLAYER WINS					 
run(Board, _Player, _GameType-_Dif) :-
								printBoard(Board),
								write('White WON').
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% CAN STILL PLAY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHECKS IF WHITE CAN STILL PLAY
canStillPlay(Board-CapW-CapB, Player):-
	Player,
	existsOnBoard(Board, 1),
	canStillPlay3(Player, Board-CapW-CapB).
	
% CHECKS IF BLACK CAN STILL PLAY
canStillPlay(Board-CapW-CapB, Player) :- 
	\+ Player,
	existsOnBoard(Board, 2),
	canStillPlay3(Player, Board-CapW-CapB).		

canStillPlay3(Player, Board-CapW-CapB):-
	getStartPositions(Player, Board-CapW-CapB, StartPositions),
	canStillPlay2(Player, Board-CapW-CapB, StartPositions, [], FinalPositions),
	length(FinalPositions, Size), Size > 0.
											   
canStillPlay2(_Player, _Board, [], FinalPositions, FinalPositions).
canStillPlay2(Player, Board, [Head|Tail], FinalPositions, FP5):-
	getAllMovePositions(Player, Board, Head, MovePositions),
	getAllJumpPositions(Player, Board, Head, JumpPositions),
	getAllCapturePositions(Player, Board, Head, CapturePositions),
	append(MovePositions, FinalPositions, FP2),
	append(JumpPositions, FP2, FP3),
	append(CapturePositions, FP3, FP4),
	canStillPlay2(Player, Board, Tail, FP4, FP5).
														
											   
% CHECKS IF AT LEAST ONE PIECE EXISTS ON THE BOARD
existsOnBoard([Line|_Tail], State) :- member(State, Line).
existsOnBoard([_Line|Tail], State) :- existsOnBoard(Tail, State).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% GETS ALL THE PIECES OF THE PLAYER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GET ALL STARTING POSITIONS FOR WHITE
getStartPositions(Player, Board-_CapW-_CapB, StartPositions) :-
	Player, findall(Row-Col, isPiece(Board, Row-Col, 1), StartPositions).

% GET ALL STARTING POSITIONS FOR BLACK
getStartPositions(Player, Board-_CapW-_CapB, StartPositions) :-
	\+ Player, findall(Row-Col, isPiece(Board, Row-Col, 2), StartPositions).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%% PROCESS MOVEMENT POSITIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
processStartPositions(Player, Board, StartPositions, StartRow-StartCol, GameType) :-
	 getStartPositions(Player, Board, StartPositions),
	 length(StartPositions, Size),
	 Size > 0,
	 chooseStartingPosition(Player, Board, StartPositions, StartRow-StartCol, GameType).
																			 																			
processMovePositions(Player, Board, StartRow-StartCol, MovePositions):-
	getAllMovePositions(Player, Board, StartRow-StartCol, MovePositions),
	write('Valid Move Destinations for Piece:'),nl,
	write(MovePositions), nl.
																		
processJumpPositions(Player, Board, StartRow-StartCol, JumpPositions):-
	 getAllJumpPositions(Player, Board, StartRow-StartCol, JumpPositions),
	 write('Valid Jump Destinations for Piece:'),nl,
	 write(JumpPositions), nl.
																		 
processCapturePositions(Player, Board, StartRow-StartCol, CapturePositions):-
	getAllCapturePositions(Player, Board, StartRow-StartCol, CapturePositions),
	write('Valide Capture Destinations for Piece:'), nl,
	write(CapturePositions),nl.
																				
chooseStartingPosition(_Player, _Board, Positions, Row-Col, GameType) :-
	GameType = 1,
	readStartingPosition(Positions, Row-Col).

chooseStartingPosition(Player, _Board, Positions, Row-Col, GameType) :-
	GameType = 2,
	Player, 
	readStartingPosition(Positions, Row-Col).
																
chooseStartingPosition(Player, Board, Positions, Row-Col, GameType) :-
	GameType = 2,
	\+Player, 
	computerChooseStartingPosition(Player, Board, Positions, Row-Col),
	write('Computer chose '), write(Row-Col),nl.
																
chooseStartingPosition(Player, Board, Positions, Row-Col, GameType) :-
	GameType = 3,
	computerChooseStartingPosition(Player, Board, Positions, Row-Col),
	write('Computer chose '), write(Row-Col),nl.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%% READ PLAYS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%																
readStartingPosition(Positions, StartRow-StartCol) :- 
	write('Starting Positions'),nl,
	write('Select Start Position'), nl,
	write(Positions), nl,
	read(StartRow-StartCol),
	checkValidChoice(StartRow-StartCol, Positions).

readPlay(_Player, GameType-_Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	GameType = 1,
	readPlayHuman(PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions).
	
readPlay(Player, GameType-_Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	GameType = 2,
	Player,
	readPlayHuman(PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions).
						
readPlay(Player, GameType-Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	GameType = 2,
	\+Player,
	computerChoosePlay(Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions).

readPlay(_Player, GameType-Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	GameType = 3,
	computerChoosePlay(Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions).
	
readPlayHuman(PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	write('Select Play EndingRow EndingColumn'), nl,
	read(PlayType-EndRow-EndCol),
	checkValidChoiceGeneral(PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% EXECUTE PLAY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOVEMENT
executePlay(Player, GameType, PlayType, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard2-NCapW-NCapB) :- 
	PlayType = 7, % movement
	movePiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard),
	savePiece(Player, GameType, NewBoard-CapW-CapB, EndRow-EndCol, NewBoard2-NCapW-NCapB).

% JUMP
executePlay(Player, GameType, PlayType, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW2-NCapB2) :- 
	PlayType = 8, % jump
	jumpPiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard),
	getAllJumpPositions(Player, NewBoard-CapW-CapB, EndRow-EndCol, JumpPositions),
	savePiece2(Player, GameType, NewBoard-CapW-CapB, EndRow-EndCol, NewBoard2-NCapW-NCapB, JumpPositions, NewJumpPositions),
	mandatoryJump(Player, GameType, 8, NewBoard2-NCapW-NCapB, EndRow-EndCol, NewJumpPositions, NewBoard3-NCapW2-NCapB2).

% CAPTURE																		
executePlay(Player, GameType, PlayType, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW3-NCapB3) :- 
	PlayType = 9, % capture
	capturePiece(Player, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard-NCapW-NCapB),
	getAllCapturePositions(Player, NewBoard-NCapW-NCapB, EndRow-EndCol, CapturePositions),
	savePiece2(Player, GameType, NewBoard-NCapW-NCapB, EndRow-EndCol, NewBoard2-NCapW2-NCapB2, CapturePositions, NewCapturePositions),
	mandatoryCapture(Player, GameType, 9, NewBoard2-NCapW2-NCapB2, EndRow-EndCol, NewCapturePositions, NewBoard3-NCapW3-NCapB3).


movePiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard2) :-
	Player,
	changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	changeBoard(NewBoard, EndRow-EndCol, 1, NewBoard2).

movePiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard2) :-
	\+Player,
	changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	changeBoard(NewBoard, EndRow-EndCol, 2, NewBoard2).
																		
jumpPiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard2) :-
	 Player,
	 changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	 changeBoard(NewBoard, EndRow-EndCol, 1, NewBoard2).

jumpPiece(Player, Board, StartRow-StartCol, EndRow-EndCol, NewBoard2) :-
	 \+Player,
	 changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	 changeBoard(NewBoard, EndRow-EndCol, 2, NewBoard2).

capturePiece(Player, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW-CapB) :-
	 Player,
	 NCapW is CapW+1,
	 getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol),
	 changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	 changeBoard(NewBoard, MiddleRow-MiddleCol, 0, NewBoard2),
	 changeBoard(NewBoard2, EndRow-EndCol, 1, NewBoard3).

capturePiece(Player, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-CapW-NCapB) :-
	 \+Player,
	 NCapB is CapB+1,
	 getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol),
	 changeBoard(Board, StartRow-StartCol, 0, NewBoard),
	 changeBoard(NewBoard, MiddleRow-MiddleCol, 0, NewBoard2),
	 changeBoard(NewBoard2, EndRow-EndCol, 2, NewBoard3).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%% MANDATORY MOVEMENT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MANDATORY JUMP
mandatoryJump(_,_,_, Board-CapW-CapB, _, [], Board-CapW-CapB).

mandatoryJump(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard3-NCapW2-NCapB2) :-
	GameType = 1,
	printBoard(Board-CapW-CapB),nl,
	mandatoryHuman(PositionList, EndRow-EndCol),
	executePlay(Player, 1, 8, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW2-NCapB2).

mandatoryJump(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard3-NCapW2-NCapB2) :-
	GameType = 2,
	Player,
	printBoard(Board-CapW-CapB),nl,
	mandatoryHuman(PositionList,EndRow-EndCol),
	executePlay(Player, 1, 8, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW2-NCapB2).

mandatoryJump(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard3-NCapW2-NCapB2) :-
	GameType = 2,
	\+Player,
	printBoard(Board-CapW-CapB),nl,
	write('Computer Mandatory Jump'),nl,
	write('Possible Destinations '), write(PositionList),nl,
	computerChoose(PositionList, EndRow-EndCol),
	write('Computer Chose '), write(EndRow-EndCol),nl,
	executePlay(Player, 2, 8, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW2-NCapB2).

mandatoryJump(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard3-NCapW2-NCapB2) :-
	GameType = 3,
	printBoard(Board-CapW-CapB),nl,
	write('Computer Mandatory Jump'),nl,
	write('Possible Destinations '), write(PositionList),nl,
	computerChoose(PositionList, EndRow-EndCol),
	write('Computer Chose '), write(EndRow-EndCol),nl,
	executePlay(Player, 3, 8, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard3-NCapW2-NCapB2).
																		
% MANDATORY CAPTURE																		
mandatoryCapture(_,_,_,Board-CapW-CapB,_,[],Board-CapW-CapB).

mandatoryCapture(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard2-NCapW2-NCapB2) :-
	GameType = 1,
	printBoard(Board-CapW-CapB),nl,
	mandatoryHuman(PositionList, EndRow-EndCol),
	executePlay(Player, 1, 9, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard2-NCapW2-NCapB2).
																	
																		
mandatoryCapture(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard2-NCapW2-NCapB2) :-
	GameType = 2,
	Player, 
	printBoard(Board-CapW-CapB),nl,
	mandatoryHuman(PositionList, EndRow-EndCol),
	executePlay(Player, 2, 9, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard2-NCapW2-NCapB2).
																		
mandatoryCapture(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard2-NCapW2-NCapB2) :-
	GameType = 2,
	\+Player,
	printBoard(Board-CapW-CapB),nl,
	write('Computer Mandatory Capture'),nl,
	write('Possible Destinations '), write(PositionList),nl,
	computerChoose(PositionList, EndRow-EndCol),
	write('Computer Chose '), write(EndRow-EndCol),nl,
	executePlay(Player, 2, 9, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard2-NCapW2-NCapB2).

mandatoryCapture(Player, GameType, _PlayType, Board-CapW-CapB, StartRow-StartCol, PositionList, NewBoard2-NCapW2-NCapB2) :-
	GameType = 3,
	printBoard(Board-CapW-CapB),nl,
	write('Computer Mandatory Capture'),nl,
	write('Possible Destinations '), write(PositionList),nl,
	computerChoose(PositionList, EndRow-EndCol),
	write('Computer Chose '), write(EndRow-EndCol),nl,
	executePlay(Player, 3, 9, Board-CapW-CapB, StartRow-StartCol, EndRow-EndCol, NewBoard2-NCapW2-NCapB2).
																	
mandatoryHuman(Positions, Row-Col):- 
	write('Mandatory -> Possible Destinations:'), nl,
	write(Positions), nl,
	read(Row-Col),
	checkValidChoice(Row-Col, Positions).
									
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%% RECOVERING PIECES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%																							 
savePiece(Player, GameType, Board-CapW-CapB, Row-Col, NewBoard2-CapW-NCapB2) :-
	Player,
	Row = 7,
	changeBoard(Board, Row-Col, 0, NewBoard),
	NCapB is CapB + 1,
	getPieceBack(Player, GameType, NewBoard-CapW-NCapB, NewBoard2-CapW-NCapB2,0).
												
savePiece(Player, GameType, Board-CapW-CapB, Row-Col, NewBoard2-NCapW2-CapB) :-
	\+Player,
	Row = 0,
	changeBoard(Board, Row-Col, 0, NewBoard),
	NCapW is CapW + 1,
	getPieceBack(Player, GameType, NewBoard-NCapW-CapB, NewBoard2-NCapW2-CapB,0).
savePiece(_, _,Board-CapW-CapB,_, Board-CapW-CapB).

savePiece2(Player, GameType, Board-CapW-CapB, Row-Col, NewBoard2-CapW-NCapB2, Positions, NewPositions) :-	
	Player,
	Row = 7,
	intersection(Positions, [], NewPositions),
	changeBoard(Board, Row-Col, 0, NewBoard),
	NCapB is CapB + 1,
	getPieceBack(Player, GameType, NewBoard-CapW-NCapB, NewBoard2-CapW-NCapB2,0).
																									
savePiece2(Player, GameType, Board-CapW-CapB, Row-Col, NewBoard2-NCapW2-CapB, Positions, NewPositions) :-	
	\+Player,
	Row = 0,
	intersection(Positions, [], NewPositions),
	changeBoard(Board, Row-Col, 0, NewBoard),
	NCapW is CapW + 1,
	getPieceBack(Player, GameType, NewBoard-NCapW-CapB, NewBoard2-NCapW2-CapB,0).
																									
savePiece2(_, _, Board-CapW-CapB,_, Board-CapW-CapB, Positions, Positions).

getPieceBack(_Player, _GameType, Board-CapW-CapB, Board-CapW-CapB, Count) :- Count = 2.

getPieceBack(Player, GameType, Board-CapW-CapB, NewBoard2-CapW-NCapB2, Count) :-
	Player,
	Count < 2,
	CapB > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, Size), Size > 0,
	chooseSpaceForPieceSave(Player, GameType, FreePositions, Row-Col),
	changeBoard(Board, Row-Col, 1, NewBoard),
	NewCount is Count + 1,
	NCapB is CapB - 1,
	getPieceBack(Player, GameType, NewBoard-CapW-NCapB, NewBoard2-CapW-NCapB2, NewCount).

getPieceBack(Player, GameType, Board-CapW-CapB, NewBoard2-NCapW2-CapB, Count) :-
	\+Player,
	Count < 2,
	CapW > 0,
	getAllFreePositions(Player, Board, FreePositions),
	length(FreePositions, Size), Size > 0,
	chooseSpaceForPieceSave(Player, GameType, FreePositions, Row-Col),
	changeBoard(Board, Row-Col, 2, NewBoard),
	NewCount is Count + 1,
	NCapW is CapW - 1,
	getPieceBack(Player, GameType, NewBoard-NCapW-CapB, NewBoard2-NCapW2-CapB, NewCount).

getPieceBack(_Player ,_GameType, Board-CapW-CapB, Board-CapW-CapB, _Count).

chooseSpaceForPieceSave(_Player, GameType, Positions, Row-Col) :- 
	GameType = 1,
	chooseSpaceHuman(Positions, Row-Col).

chooseSpaceForPieceSave(Player, GameType, Positions, Row-Col) :- 
	GameType = 2,
	Player,
	chooseSpaceHuman(Positions, Row-Col).
															
chooseSpaceForPieceSave(Player, GameType, Positions, Row-Col) :- 
	GameType = 2,
	\+Player, 
	write('Computer Saving Piece'),nl,
	write('Possible Choices '), write(Positions),nl,
	computerChoose(Positions, Row-Col),
	write('Computer Chose: '), write(Row-Col),nl.

chooseSpaceForPieceSave(_Player, GameType, Positions, Row-Col) :-  
	GameType = 3,
	write('Computer Saving Piece'),nl,
	write('Possible Choices '), write(Positions),nl,
	computerChoose(Positions, Row-Col),
	write('Computer Chose: '), write(Row-Col),nl.
	
chooseSpaceHuman(FreePositions, Row-Col):-
	repeat,
	write('Recovering Piece...'),nl,
	write('Available Positions For Recovered Piece:'),nl,
	write(FreePositions),nl,
	write('Choose Position:'),nl,
	read(Row-Col),
	checkValidChoice(Row-Col, FreePositions).
	
intersection(_List, [], []).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%% COMPUTER PREDICATES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
computerChooseStartingPosition(Player, Board, Positions, Row-Col):-
	computerChoose(Positions, Row-Col),
	choose2(Player, Board, Row-Col),
	write('Computer Starting Position: ') ,write(Row-Col),nl.				
	
choose2(Player, Board, Row-Col) :-
	getAllMovePositions(Player, Board, Row-Col, MovePositions),
	length(MovePositions, L1), L1 > 0.
	
choose2(Player, Board, Row-Col) :-
	getAllJumpPositions(Player, Board, Row-Col, JumpPositions),
	length(JumpPositions, L2), L2 > 0.
	
choose2(Player, Board, Row-Col) :-
	getAllCapturePositions(Player, Board, Row-Col, CapturePositions),
	length(CapturePositions, L3), L3 > 0.

computerChoosePlay(Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	Dif = 1,
	repeat,
		random(7,10,PlayType),																								
		computerGetPlay(PlayType, EndRow-EndCol, MovePositions, JumpPositions, CapturePositions),
		write('Computer Chose: '), write(PlayType),
		write('  Moving To: '), write(EndRow-EndCol),nl.
																								
computerChoosePlay(Dif, PlayType-EndRow-EndCol, _MovePositions, _JumpPositions, CapturePositions):-
	Dif = 2,
	length(CapturePositions, Size), Size > 0,
	computerChoose(CapturePositions, EndRow-EndCol),
	PlayType is 9,
	write('Computer Chose: '), write('Capture'),
	write('  Moving To: '), write(EndRow-EndCol),nl.
																							
computerChoosePlay(Dif, PlayType-EndRow-EndCol, MovePositions, JumpPositions, CapturePositions):-
	Dif = 2,
	repeat,
		random(7,10,PlayType),																								
		computerGetPlay(PlayType, EndRow-EndCol, MovePositions, JumpPositions, CapturePositions),
		write('Computer Chose: '), write(PlayType),
		write('  Moving To: '), write(EndRow-EndCol),nl.
																								
computerGetPlay(_PlayType, _EndRow-_EndCol, [], [], []).

computerGetPlay(PlayType, EndRow-EndCol, MovePositions, _JumpPositions, _CapturePositions):-
	PlayType = 7,
	computerChoose(MovePositions, EndRow-EndCol),
	checkValidChoice(EndRow-EndCol, MovePositions).
	
computerGetPlay(PlayType, EndRow-EndCol, _MovePositions, JumpPositions, _CapturePositions):-
	PlayType = 8,
	computerChoose(JumpPositions, EndRow-EndCol),
	checkValidChoice(EndRow-EndCol, JumpPositions).
	
computerGetPlay(PlayType, EndRow-EndCol, _MovePositions, _JumpPositions, CapturePositions):-
	PlayType = 9,
	computerChoose(CapturePositions, EndRow-EndCol),
	checkValidChoice(EndRow-EndCol, CapturePositions).
																								
computerChoose([],_).
computerChoose(Positions, Row-Col) :- 
	length(Positions, Length),
	random(0, Length, Index),
	nth0(Index, Positions, Row-Col).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%% GET ALL THE POSSIBLE DESTINATIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GETS ALL THE POSITIONS A PIECE CAN MOVE TO
getAllMovePositions(Player, Board, Row-Col, MovePositions) :- findall(NewRow-NewCol, checkMovePosition(Player, Board, Row-Col, NewRow-NewCol), MovePositions). 

% GETS ALL THE POSITIONS A PIECE CAN JUMP TO
getAllJumpPositions(Player, Board, Row-Col, JumpPositions) :- findall(NewRow-NewCol, checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol), JumpPositions).

% GETS ALL THE POSITIONS A PIECE CAN CAPTURE TO
getAllCapturePositions(Player, Board, Row-Col, CapturePositions) :- findall(NewRow-NewCol, checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol), CapturePositions).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% RETRIVES MOVEMENT DESTINATIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHECKS THE POSITION FOR THE WHITE PIECES
% CHECKS UP
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+1,
													        NewCol is Col,
													        checkEmpty(Board, NewRow-NewCol).
% CHECKS UP RIGHT
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+1,
													        NewCol is Col+1,
													        checkEmpty(Board, NewRow-NewCol).	
% CHECKS UP LEFT											
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+1,
													        NewCol is Col-1,
													        checkEmpty(Board, NewRow-NewCol).
% CHECKS THE POSITION FOR THE BLACK PIECES
% CHECKS UP
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-1,
													        NewCol is Col,
													        checkEmpty(Board, NewRow-NewCol).
% CHECKS UP RIGHT															
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-1,
													        NewCol is Col+1,
													        checkEmpty(Board, NewRow-NewCol).	
% CHECKS UP LEFT
checkMovePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-1,
													        NewCol is Col-1,
													        checkEmpty(Board, NewRow-NewCol).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% RETRIEVES JUMP DESTINATIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHECKS JUMP POSITION FOR WHITE PIECES
% CHECKS UP
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+2, NewCol is Col, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row+1, MiddleCol is Col, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS UP LEFT												
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+2, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row+1, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS UP RIGHT							
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- Player,
															NewRow is Row+2, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row+1, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS JUMP POSITION FOR BLACK PIECES
% CHECKS UP
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-2, NewCol is Col, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row-1, MiddleCol is Col, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS UP LEFT												
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-2, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row-1, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS UP RIGHT							
checkJumpPosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															NewRow is Row-2, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
															MiddleRow is Row-1, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 2).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%% RETRIEVES CAPTURE DESTINATIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CHECKS THE CAPTURE POSITION FOR THE WHITE PIECES -> 5 DIRECTIONS
% CHECKS UP
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- 	Player,
																NewRow is Row+2, NewCol is Col, checkEmpty(Board, NewRow-NewCol),
																MiddleRow is Row+1, MiddleCol is Col, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS LEFT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- 	Player,
																NewRow is Row, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
																MiddleRow is Row, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS RIGHT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- 	Player,
																NewRow is Row, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
																MiddleRow is Row, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS UP LEFT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- 	Player,
																NewRow is Row+2, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
																MiddleRow is Row+1, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 2).
% CHECKS UP RIGHT															   
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- 	Player,
																NewRow is Row+2, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
																MiddleRow is Row+1, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 2).	

% CHECKS THE CAPTURE POSITION FOR THE BLACK PIECES -> 5 DIRECTIONS
% CHECKS UP
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															   NewRow is Row-2, NewCol is Col, checkEmpty(Board, NewRow-NewCol),
															   MiddleRow is Row-1, MiddleCol is Col, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS LEFT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															   NewRow is Row, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
															   MiddleRow is Row, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS RIGHT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															   NewRow is Row, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
															   MiddleRow is Row, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS UP LEFT
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															   NewRow is Row-2, NewCol is Col-2, checkEmpty(Board, NewRow-NewCol),
															   MiddleRow is Row-1, MiddleCol is Col-1, checkPiece(Board, MiddleRow-MiddleCol, 1).
% CHECKS UP RIGHT															   
checkCapturePosition(Player, Board, Row-Col, NewRow-NewCol) :- \+Player,
															   NewRow is Row-2, NewCol is Col+2, checkEmpty(Board, NewRow-NewCol),
															   MiddleRow is Row-1, MiddleCol is Col+1, checkPiece(Board, MiddleRow-MiddleCol, 1).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%% CHECKS TILE ON BOARD FOR PIECE OR LACK THEREOF %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
checkPiece(Board-_CapByW-_CapByB, Row-Col, Piece) :- isPiece(Board, Row-Col, Piece).

isPiece(Board, Row-Col, Piece) :- nth0(Row, Board, ColList), nth0(Col, ColList, Piece).

checkEmpty(Board-_CapByW-_CapByB, Row-Col) :- isEmpty(Board, Row-Col).								  

isEmpty(Board, Row-Col) :- nth0(Row, Board, ColList), nth0(Col, ColList, 0).   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%% CHOICE VALIDATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
checkValidChoiceGeneral(PlayType-EndRow-EndCol, MovePositions, _JumpPositions, _CapturePositions):- PlayType = 7, checkValidChoice(EndRow-EndCol, MovePositions).

checkValidChoiceGeneral(PlayType-EndRow-EndCol, _MovePositions, JumpPositions, _CapturePositions):- PlayType = 8, checkValidChoice(EndRow-EndCol, JumpPositions).

checkValidChoiceGeneral(PlayType-EndRow-EndCol, _MovePositions, _JumpPositions, CapturePositions):- PlayType = 9, checkValidChoice(EndRow-EndCol, CapturePositions).

checkValidChoice(Row-Col, List) :- member(Row-Col, List).						  								
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% GET THE MIDDLE POSITIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% FOR THE WHITE PIECES
% RIGHT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																			Player, StartRow = EndRow, EndCol > StartCol, MiddleRow is StartRow, MiddleCol is StartCol+1.
% LEFT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																			Player, StartRow = EndRow, EndCol < StartCol, MiddleRow is StartRow, MiddleCol is StartCol-1.
% UP
getMiddlePosition(Player, StartRow-StartCol, _EndRow-EndCol, MiddleRow-MiddleCol):-
																			Player, StartCol = EndCol, MiddleRow is StartRow+1, MiddleCol is StartCol.
% UP RIGHT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																			Player, StartRow < EndRow, StartCol < EndCol, MiddleRow is StartRow+1, MiddleCol is StartCol+1.
% UP LEFT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																			Player, StartRow < EndRow, StartCol > EndCol, MiddleRow is StartRow+1, MiddleCol is StartCol-1.
																			
% FOR THE BLACK PIECES
% RIGHT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																				\+Player,  StartRow = EndRow, EndCol > StartCol,  MiddleRow is StartRow, MiddleCol is StartCol+1.
% LEFT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																				\+Player, StartRow = EndRow, EndCol < StartCol, MiddleRow is StartRow, MiddleCol is StartCol-1.
% UP
getMiddlePosition(Player, StartRow-StartCol, _EndRow-EndCol, MiddleRow-MiddleCol):-
																				\+Player, StartCol = EndCol, MiddleRow is StartRow-1, MiddleCol is StartCol.
% UP RIGHT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																				\+Player, StartRow > EndRow, StartCol < EndCol, MiddleRow is StartRow-1, MiddleCol is StartCol+1.
% UP LEFT
getMiddlePosition(Player, StartRow-StartCol, EndRow-EndCol, MiddleRow-MiddleCol):-
																				\+Player, StartRow > EndRow, StartCol > EndCol, MiddleRow is StartRow-1, MiddleCol is StartCol-1.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%% PROCESSES THE FREE SPACES AT FIRST AND LAST ROW %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
getAllFreePositions(Player, Board, FreePositions) :- findall(Row-Col, checkFreePositions(Player, Board, Row-Col), FreePositions). 

% WHITE
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 1, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 2, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 3, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 4, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 5, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 0, Col is 6, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 1, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 2, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 3, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 4, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 5, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- Player, Row is 1, Col is 6, isEmpty(Board, Row-Col).

% BLACK
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 1, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 2, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 3, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 4, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 5, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 7, Col is 6, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 1, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 2, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 3, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 4, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 5, isEmpty(Board, Row-Col).
checkFreePositions(Player, Board, Row-Col) :- \+Player, Row is 6, Col is 6, isEmpty(Board, Row-Col).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%% BOARD MODIFICATION FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
changeBoard(Board, Row-Col, Piece, NewBoard) :-
												changeBoard2(Board, 0, Col, Piece, Row, NewBoard),!.
					
changeBoard2([Head|Tail], FinalRow, Col , Piece, FinalRow, [NewHead|Tail]) :- replace(Head, Col, Piece, NewHead).
					
changeBoard2([Head|Tail], Row, Col , Piece, FinalRow, [Head|NewTail]) :- Row \= FinalRow,	
																		 NewRow is Row+1,
																		 changeBoard2(Tail, NewRow, Col, Piece, FinalRow, NewTail).
																		   														   
replace([_|Tail], 0, Element, [Element|Tail]).
replace([Head|Tail], Col, Element, [Head|Rest]):- Col > 0, NewCol is Col-1, replace(Tail, NewCol, Element, Rest), !.
replace(L, _, _, L).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%