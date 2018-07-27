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
		grid[0][0].style.backgroundImage="url('images/BlackRook.png')";
		grid[0][1].style.backgroundImage="url('images/BlackKnight.png')";
		grid[0][2].style.backgroundImage="url('images/BlackBishop.png')";
		grid[0][3].style.backgroundImage="url('images/BlackQueen.png')";
		grid[0][4].style.backgroundImage="url('images/BlackKing.png')";
		grid[0][5].style.backgroundImage="url('images/BlackBishop.png')";
		grid[0][6].style.backgroundImage="url('images/BlackKnight.png')";
		grid[0][7].style.backgroundImage="url('images/BlackRook.png')";

		for (var i = 0; i < 8; i++)
		{
			grid[1][i].style.backgroundImage="url('images/BlackPawn.png')";
			grid[6][i].style.backgroundImage="url('images/WhitePawn.png')";
		}

		grid[7][0].style.backgroundImage="url('images/WhiteRook.png')";
		grid[7][1].style.backgroundImage="url('images/WhiteKnight.png')";
		grid[7][2].style.backgroundImage="url('images/WhiteBishop.png')";
		grid[7][3].style.backgroundImage="url('images/WhiteQueen.png')";
		grid[7][4].style.backgroundImage="url('images/WhiteKing.png')";
		grid[7][5].style.backgroundImage="url('images/WhiteBishop.png')";
		grid[7][6].style.backgroundImage="url('images/WhiteKnight.png')";
		grid[7][7].style.backgroundImage="url('images/WhiteRook.png')";
		 
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



