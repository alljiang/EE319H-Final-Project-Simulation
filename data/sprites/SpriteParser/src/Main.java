import sun.awt.windows.WPrinterJob;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class Main
{

    static String filename = "gameandwatch_minimal.png";
    static Pixel background = new Pixel(0xff882c3f);
    static int cropTolerance = 3;  // pixels
    static int minimumDiagonalLength = 1;

    static int width, height;

    static Pixel[][] img;
    static boolean[][] visited;
    static boolean[][] processed;
    static int[][] tolerance;
    static ArrayList<Point> list = new ArrayList<>();

    public static void main(String[] args) throws IOException
    {
        BufferedImage bi = ImageIO.read(new File(filename));

        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        ImageIO.write(bi, "png", stream);

        width = bi.getWidth();
        height = bi.getHeight();
        img = new Pixel[height][width];
        visited = new boolean[height][width];
        processed = new boolean[height][width];

        for(int h = 0; h < height; h++) {
            for(int w = 0; w < width; w++) {
                int rgb = bi.getRGB(w, h);
                img[h][w] = new Pixel(rgb);
            }
        }

        System.out.println(img[0][0]);

        int imageCounter = 0;
        for(int r = 0; r < height; r++) {
            for(int c = 0; c < width; c++) {
                if(visited[r][c]) continue;
                if(img[r][c].equals(background)) {
                    visited[r][c] = true;
                    continue;
                }

                // recursively get all the points
                list.clear();
                tolerance = new int[height][width];
                FF(r, c, cropTolerance);

                int lowR = Integer.MAX_VALUE;
                int lowC = Integer.MAX_VALUE;
                int highR = Integer.MIN_VALUE;
                int highC = Integer.MIN_VALUE;
                for(Point p : list) {
                    int row = p.x;
                    int col = p.y;
                    lowR = Math.min(row, lowR);
                    lowC = Math.min(col, lowC);
                    highR = Math.max(row, highR);
                    highC = Math.max(col, highC);
                }
                if(Point2D.distance(lowR, lowC, highR, highC) < minimumDiagonalLength) continue;
                BufferedImage newImage = new BufferedImage(
                        highC - lowC+1, highR - lowR+1, BufferedImage.TYPE_INT_ARGB);
                boolean write = true;
                for(int row = 0; row <= highR - lowR; row++) {
                    for(int col = 0; col <= highC - lowC; col++) {
                        if(img[lowR + row][lowC + col].equals(background))
                            newImage.setRGB(col, row, 0x00000000);
                        else if(processed[lowR+row][lowC+col]) {
                            write = false;
                            break;
                        }
                        else {
                            newImage.setRGB(col, row, img[lowR + row][lowC + col].rgb);
                            processed[lowR+row][lowC+col] = true;
                        }
                    }
                    if(!write) break;
                }
                if(write) {
                    ImageIO.write(newImage, "png", new File("cropped/" + imageCounter + ".png"));
                    imageCounter++;
                }
            }
        }
    }

    static int[] dr = {0, 1, 0, -1};
    static int[] dc = {-1, 0, 1, 0};

    // floodfill to get all of the image
    static void FF(int r, int c, int toleranceAllowed) {
        if(r < 0 || c < 0 || r >= height || c >= width || (visited[r][c] && toleranceAllowed <= tolerance[r][c]))
            return;
        visited[r][c] = true;
        tolerance[r][c] = toleranceAllowed;
        if(img[r][c].equals(background)) {
            if(toleranceAllowed > 0) toleranceAllowed--;
            else return;
        }
        else list.add(new Point(r, c));


        for(int i = 0; i < 3; i++) {
            FF(r + dr[i], c + dc[i], toleranceAllowed);
        }
    }

}

class Pixel
{

    short r, g, b, a;  // alpha: low is transparent, high is opaque
    int rgb;

    Pixel(int rgb)
    {
        a = (short) ((rgb & 0xFF000000 >>> 24));
        r = (short) ((rgb & 0x00FF0000) >> 16);
        g = (short) ((rgb & 0x0000FF00) >> 8);
        b = (short) ((rgb & 0x000000FF));
        this.rgb = rgb;
    }

    public String toString()
    {
        return a + " " + r + " " + g + " " + b + " " + Integer.toUnsignedString(rgb, 16);
    }

    public boolean equals(Pixel p)
    {
        return this.rgb == p.rgb;
    }
}