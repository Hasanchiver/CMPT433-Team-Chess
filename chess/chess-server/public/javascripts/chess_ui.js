"use strict";
// Client-side interactions with the browser for web interface

// Make connection to server when web page is fully loaded.
var socket = io.connect();
var volume = 0;
var tempo = 0;
var serverErr = null;

$(document).ready(function() {

	var turnCounter;
	$('#error-box').hide(); 

	//create Chess board
	var grid = new Array(8);
	for (var i = 0; i < 8; i++)
	{
		grid[i] = new Array(8);
	}
	
	var table = document.createElement("table");
	for (var row = 0; row < 8; row++) {
	    var tr = document.createElement('tr');
	    for (var col = 0; col < 8; col++) {
	        var td = document.createElement('td');
	        if (row%2 == col%2) {
	            td.className = "white";
	        } else {
	            td.className = "black";
	        }
		grid[row][col] = td;
	        tr.appendChild(td);
	    }
	    table.appendChild(tr);
	}
	resetBoard();
	document.body.appendChild(table);

	//this queries the table every one second for current board
	setInterval(function () {
		socket.emit('board');
	}, 1000);
	

	

	//handle incoming messages from the server
	socket.on('turn-reply', function(message){		
		$('#error-box').hide();
		changeTurn(message);
		
	});
	socket.on('board-reply', function(message){
		$('#error-box').hide();
		console.log(message);
		setCurrentBoard(message);
	});



	
	//Populate initial board
	function resetBoard(){
		setUnitAt("BlackRook"  , 0, 0);
		setUnitAt("BlackKnight", 0, 1);
		setUnitAt("BlackBishop", 0, 2);
		setUnitAt("BlackQueen" , 0, 3);
		setUnitAt("BlackKing"  , 0, 4);
		setUnitAt("BlackBishop", 0, 5);
		setUnitAt("BlackKnight", 0, 6);
		setUnitAt("BlackRook"  , 0, 7);
		
		for (var i = 0; i < 8; i++)
		{
			setUnitAt("BlackPawn", 1, i);
			setUnitAt("WhitePawn", 6, i);
		}

		setUnitAt("WhiteRook"  , 7, 0);
		setUnitAt("WhiteKnight", 7, 1);
		setUnitAt("WhiteBishop", 7, 2);
		setUnitAt("WhiteQueen" , 7, 3);
		setUnitAt("WhiteKing"  , 7, 4);
		setUnitAt("WhiteBishop", 7, 5);
		setUnitAt("WhiteKnight", 7, 6);
		setUnitAt("WhiteRook"  , 7, 7);
		 
	}

	function changeTurn(newTurn){
		turnCounter = Number(newTurn);
		$('#turnSpan').text(turnCounter);	
	}


	function setCurrentBoard(message)
	{
		var res = message.split("/");
		for (var row = 0; row < 8; row++)
		{
			var col = 0;
			for (var j = 0; j < res[row].length; j++)
			{
				var res2 = res[row].split("");
				if(isNaN(res2[j]))
				{
					var temp = processChar(res2[j]);
					setUnitAt(temp, row, col);
					col++;
				}
				else
				{
					var temp = parseInt(res2[j],10);
					for (var i = 0; i < temp; i++)
					{
						clearSquareAt(row,col);
						col++;
					}	

				}
			}
		}
	}

	function setUnitAt(unit, row, col)
	{
		grid[row][col].style.backgroundImage="url('images/" + unit + ".png')";
	}

	function clearSquareAt(row, col)
	{
		grid[row][col].style.backgroundImage="none";
	}

	function processChar(char)
	{
		var unit, color, fullName;
		if (char == char.toLowerCase()){
			color = "Black";
		}
		else
		{
			color = "White";
		}
		switch(char.toLowerCase()) {
			case 'r':
				unit = "Rook";
				break;
			case 'n':
				unit = "Knight";
				break;
			case 'b':
				unit = "Bishop";
				break;
			case 'k':
				unit = "King";
				break;
			case 'q':
				unit = "Queen";
				break;
			case 'p':
				unit = "Pawn";
				break;
			default:
				break;
		}
		fullName = color + unit;
		return fullName;
	}
});



