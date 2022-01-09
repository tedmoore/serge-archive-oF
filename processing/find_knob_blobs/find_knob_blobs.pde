int dist_thresh = 14;

void setup() {
  size(1000, 1000);
 
  processFolder( new File(dataPath("../../../bin/data/images/Serge GUI Layout/3-PANELS/Knobs Only/")) );
  processFolder( new File(dataPath("../../../bin/data/images/Serge GUI Layout/UMAP CONTROLLER/Knobs Only/")) );

  exit();
}

void processFolder(File folder){
  
  File[] files = folder.listFiles();

  for (File f : files) {
    String fs = f.toString();
    if (fs.endsWith(".png") && !fs.endsWith("knob_positions.png")) {
      println(f.toString());
      findBlobs(f);
    }
  }
}

void findBlobs(File file) {
  PImage knobs = loadImage(file.toString());
  knobs.loadPixels();

  String[] tokens = split(file.getName(), ".");
  String output_path = file.getParent().toString() + "/" + tokens[0].replace(" KNOBS ONLY", "") + "_knob_positions";
  PrintWriter output = createWriter(output_path + ".csv");
  PGraphics img = createGraphics(knobs.width, knobs.height);

  String[] atTokens = split(tokens[0], "@");

  int mult = int(atTokens[1].charAt(0)) - 48;

  //println(atTokens[1]);
  //println(atTokens[1].charAt(0));
  //println("mult: " + mult);

  ArrayList<Blob> blobs = new ArrayList<Blob>();

  for (int y = 0; y < knobs.height; y++) {
    for (int x = 0; x < knobs.width; x++) {
      int pix_i = (y * knobs.width) + x;
      if (brightness(knobs.pixels[pix_i]) < 255) {
        // non white pixel found
        PVector pt = new PVector(x, y);
        boolean added_to_existing_blob = false;

        for (Blob b : blobs) {
          if (b.isNear(pt, dist_thresh * mult)) {
            added_to_existing_blob = true;
            break;
          }
        }

        if (!added_to_existing_blob) {
          blobs.add(new Blob(pt));
        }
      }
    }
  }

  img.beginDraw();
  img.image(knobs, 0, 0);

  for (int i = 0; i < blobs.size(); i++) {
    blobs.get(i).display(1, dist_thresh * mult * 2, str(i), img);
    output.println(blobs.get(i).centroid.x + "," + blobs.get(i).centroid.y);
  }
  img.endDraw();
  img.save(output_path + ".png");

  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  println("number of blobs: " + blobs.size());
}
