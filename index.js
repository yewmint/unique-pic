/**
* @file index.js
* @author yewmint
*/

const { resolve, basename, extname } = require('path')
const fs = require('fs')
const { execSync } = require('child_process')
const _ = require('lodash')

const { mkdirSync, existsSync } = fs

function mkdir (path){
  if (!existsSync(path)) mkdirSync(path)
}

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
      let tmpFiles = getImagePaths(p)
      files = files.concat(tmpFiles)
    }
    else if (state.isFile()){
      ext = _.toLower(extname(p))
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
* move groups out of album into dir
* @param {string[][]} groups paths to groups
* @param {string} dir path to dir storing groups
*/
const moveGroups = function (groups, dir){
  let root = resolve(__dirname, dir)
  mkdir(root)

  let stores = _.chunk(groups, 100)
  stores.forEach((store, idx) => {
    let storePath = resolve(root, _.padStart(`${idx}`, 3, '0'))
    mkdir(storePath)

    store.forEach((group, idx) => {
      let prefix = `${idx}-`

      let img = _.maxBy(group, im => im[0])

      let ext = extname(img[1])
      let name = _.padStart(`${idx}`, 2, '0')
      let path = resolve(storePath, `${name}${ext}`)

      fs.renameSync(img[1], path)

      // group.forEach((img, idx) => {
      //   let ext = extname(img)
      //   let name = `${prefix}${idx}${ext}`
      //   let path = resolve(storePath, name)

      //   fs.renameSync(img, path)
      // })
    })
  })
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

  let hamming = 2

  // launch compare with image paths
  console.log('Analysing album, this may take some time...')
  let st = new Date
  execSync(`compare ${hamming} imgs.lines`)
  console.log('Done, time cost: ', (new Date - st) / 1000, 's')

  // retrieve paths of duplicates from file and move them out of album
  let groups = readLines('groups.lines')
  groups = _.compact(groups.join('\n').split('------'))
  groups = _.compact(
    groups.map(
      pathStr => _.chunk(_.compact(
        pathStr.split(/\n+/)
      ), 2)
    )
  )

  moveGroups(groups, './result')
}

run()

// let groups = readLines('groups.lines')
// console.log(_.compact(_.without(groups, '------')).length)
// groups = _.compact(groups.join('\n').split('------'))
// groups = _.compact(groups.map(pathStr => pathStr.split(/\n+/)))
// console.log(groups.length)

// let groups = readLines('groups.lines')
// groups = _.compact(groups.join('\n').split('------'))
// groups = _.compact(groups.map(pathStr => _.compact(pathStr.split(/\n+/))))
// let group = _.filter(groups, g => g.length > 1)
// console.log(group)