"use strict";
// Client-side interactions with the browser for web interface

// Make connection to server when web page is fully loaded.

var volume = 0;
var tempo = 0;
var serverErr = null;

$(document).ready(function() {

	$('#error-box').hide(); 
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
	populateBoard();
	document.body.appendChild(table);




	//Populate initial board
	function populateBoard(){
		grid[0][0].style.backgroundImage="url('images/WhiteRook.png')";
		grid[0][1].style.backgroundImage="url('images/WhiteKnight.png')";
		grid[0][2].style.backgroundImage="url('images/WhiteBishop.png')";
		grid[0][3].style.backgroundImage="url('images/WhiteKing.png')";
		grid[0][4].style.backgroundImage="url('images/WhiteQueen.png')";
		grid[0][5].style.backgroundImage="url('images/WhiteBishop.png')";
		grid[0][6].style.backgroundImage="url('images/WhiteKnight.png')";
		grid[0][7].style.backgroundImage="url('images/WhiteRook.png')";

		for (var i = 0; i < 8; i++)
		{
			grid[1][i].style.backgroundImage="url('images/WhitePawn.png')";
			grid[6][i].style.backgroundImage="url('images/BlackPawn.png')";
		}

		grid[7][0].style.backgroundImage="url('images/BlackRook.png')";
		grid[7][1].style.backgroundImage="url('images/BlackKnight.png')";
		grid[7][2].style.backgroundImage="url('images/BlackBishop.png')";
		grid[7][3].style.backgroundImage="url('images/BlackKing.png')";
		grid[7][4].style.backgroundImage="url('images/BlackQueen.png')";
		grid[7][5].style.backgroundImage="url('images/BlackBishop.png')";
		grid[7][6].style.backgroundImage="url('images/BlackKnight.png')";
		grid[7][7].style.backgroundImage="url('images/BlackRook.png')";
		 
	}
	function movePiece(from, to){
		var imageURL = from.style.backgroundImage;
		to.style.backgroundImage = imageURL;
		from.style.backgroundImage = "none";
	}
});



