import { applyRootStyles } from './src/utils.js';
import { GameBoard } from './src/game-board.js';
import { rootStyles, keyCodes } from './src/config.js';

applyRootStyles(rootStyles);
const gameBoard = new GameBoard(document.querySelector('#game-board'));

const $sidePanel = document.querySelector('#side-panel');

document.addEventListener('keydown', function (event) {
    if (keyCodes.up.includes(event.code)) {
        gameBoard.enableTile(4, 5);
        console.log('up');
    }
    if (keyCodes.right.includes(event.code)) {
        gameBoard.disableTile(4, 5);
        console.log('right');
    }
    if (keyCodes.down.includes(event.code)) {
        console.log('down');
    }
    if (keyCodes.left.includes(event.code)) {
        console.log('left');
    }
});
