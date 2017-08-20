/**
* @file index.js
* @author yewmint
*/

const { resolve, basename, extname } = require('path')
const fs = require('fs')
const { execSync } = require('child_process')

/**
* get paths to all images in dir
* @param {string} dir
* @return {string[]} paths
*/
const getImagePaths = function (dir){
  let files = []
  let paths = fs.readdirSync(dir)
  for (let p of paths){
    p = resolve(dir, p)
    let state = fs.statSync(p)
    if (state.isDirectory()){
      let tmpFiles = getFiles(p)
      files = files.concat(tmpFiles)
    }
    else if (state.isFile()){
      ext = extname(p)
      if (ext === '.jpg' || ext === '.png'){
        files.push(p)
      }
    }
  }
  return files
}

/**
* write lines into path
* @param {string[]} lines
* @param {string} path
*/
const writeLines = function (lines, path){
  let content = ''
  for (let line of lines){
    content += `${line}\n`
  }
  fs.writeFileSync(path, content)
}

/**
* read lines from path
* @param {string} path
* @return {string[]}
*/
const readLines = function (path){
  let content = fs.readFileSync(path, 'utf8')
  let lines = []
  for (let line of content.split('\n')){
    // if compare project is compiled by vc, \r need to be cleared
    if (line.indexOf('\r') !== -1){
      line = line.slice(0, -1)
    }
    if (line.length > 0){
      lines.push(line)
    }
  }
  return lines
}

/**
* move dulicates out of album into dir
* @param {string[]} dups paths to duplicates
* @param {string} dir path to dir storing duplicates
*/
const moveDuplicates = function (dups, dir){
  for (let dup of dups){
    let bn = basename(dup)
    fs.renameSync(dup, resolve(__dirname, dir, bn))
  }
}

/**
* run the program
* using 'node index.js album'
*/
const run = function (){
  if (process.argv.length !== 3) {
    throw new Error('Error: invalid arguments.')
  }

  // write paths into file
  let albumDir = resolve(__dirname, process.argv[2])
  let imagePaths = getImagePaths(albumDir)
  writeLines(imagePaths, 'imgs.lines')

  // launch compare with image paths
  // let st = new Date
  execSync('compare imgs.lines')
  // console.log((new Date - st) / 1000)

  // retrieve paths of duplicates from file and move them out of album
  let dups = readLines('dups.lines')
  moveDuplicates(dups, './duplicates')
}

run()
