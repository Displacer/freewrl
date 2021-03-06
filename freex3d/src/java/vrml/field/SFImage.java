//AUTOMATICALLY GENERATED BY genfields.pl.
//DO NOT EDIT!!!!

package vrml.field;
import vrml.*;
import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.IOException;

public class SFImage extends Field {
     int width;
     int height;
     int components;
     byte[] pixels;

    public SFImage() { }

    public SFImage(int width, int height, int components, byte[] pixels) {
	        this.width = width;
        this.height = height;
        this.components = components;
        this.pixels = pixels;
    }

    public int getWidth() {
        __updateRead();
        return width;
    }

    public int getHeight() {
        __updateRead();
        return height;
    }

    public int getComponents() {
        __updateRead();
        return components;
    }

    public byte[] getPixels() {
        __updateRead();
        return pixels;
    }

    public void setValue(int width, int height, int components, byte[] pixels) {
        this.width = width;
        this.height = height;
        this.components = components;
        this.pixels = pixels;
        __updateWrite();
    }


    public void setValue(ConstSFImage sfImage) {
        sfImage.__updateRead();
        width = sfImage.width;
        height = sfImage.height;
        components = sfImage.components;
        pixels = sfImage.pixels;
        __updateWrite();
    }

    public void setValue(SFImage sfImage) {
        sfImage.__updateRead();
        width = sfImage.width;
        height = sfImage.height;
        components = sfImage.components;
        pixels = sfImage.pixels;
        __updateWrite();
    }


    public String toString() {
        __updateRead();
        StringBuffer sb = new StringBuffer();
        sb.append(width).append(' ').append(height).append(' ').append(components);
        for (int i = 0; i < pixels.length; i+=components) {
	    sb.append(" 0x");
	    for (int j = i; j < i+components; j++)
		sb.append("0123456789ABCDEF".charAt((pixels[i+j] & 0xf0) >> 4))
		    .append("0123456789ABCDEF".charAt(pixels[i+j] & 0x0f));
	}
        return sb.toString();
    }

    public void __fromPerl(BufferedReader in)  throws IOException {

	//System.out.println ("fromPerl, Image");
		width = Integer.parseInt(in.readLine());
        	height = Integer.parseInt(in.readLine());
        	components = Integer.parseInt(in.readLine());
        	pixels = new byte[height*width*components];
	//System.out.println ("JavaClass -- fix method to read in pixels");
        	// pixels = String.getBytes(pst);

    }

    public void __toPerl(PrintWriter out)  throws IOException {
        out.print(width+" "+height+" "+components+" "+pixels);
	//out.println();
    }
    //public void setOffset(String offs) { this.offset = offs; } //JAS2
    //public String getOffset() { return this.offset; } //JAS2
}