<script lang="ts" setup>
import { ref, onMounted, watch, onBeforeUnmount } from "vue";
const emit = defineEmits(["onChange"]);

const prop = defineProps({
  config: {
    type: Object,
    default: () => {}
  },
  id: {
    type: String,
    default: "editor"
  },
  content: {
    type: String,
    default: ""
  }
});

const classicEditor = ref<Promise<any> | any>();
const editorContent = ref(prop.content);
const interval = ref<any>(null);
let editorInstance: any = null;

onMounted(() => {
  prepareCKEditor();
});

onBeforeUnmount(() => {
  if (editorInstance) {
    editorInstance.destroy();
    classicEditor.value = null;
  }
});

watch(editorContent, (value: any) => {
  if (interval.value) {
    clearTimeout(interval.value);
  }

  interval.value = setTimeout(() => {
    emit("onChange", value);
  }, 500);
});

const prepareCKEditor = async () => {
  if (typeof window !== "undefined") {
    const { default: ClassicEditor } = await import(
      "@ckeditor/ckeditor5-build-classic"
    );
    const editorElement: HTMLElement | null = document.querySelector(
      `#${prop.id}`
    );
    if (ClassicEditor && editorElement) {
      classicEditor.value = ClassicEditor.create(
        editorElement,
        prop.config && {
          ...prop.config

          /** NEEDED FEATURE WILL GO HERE  */
          // toolbar: [ 'heading', '|', 'bold', 'italic', 'link', 'bulletedList', 'numberedList', 'blockQuote' ],
          // heading: {
          //     options: [
          //         { model: 'paragraph', title: 'Paragraph', class: 'ck-heading_paragraph' },
          //         { model: 'heading1', view: 'h1', title: 'Heading 1', class: 'ck-heading_heading1' },
          //         { model: 'heading2', view: 'h2', title: 'Heading 2', class: 'ck-heading_heading2' }
          //     ]
          // }
        }
      )
        .then((editor: any) => {
          editorInstance = editor;

          editor.model.document.on("change:data", () => {
            if (editor) {
              editorContent.value = editor.getData();
            }
          });

          editor.setData(editorContent.value);
          return editor;
        })
        .catch((error: Error) => {
          console.error("[failed at prepareCKEditor]", { error });
        });
    }
  }
};
</script>
<template>
  <div class="ckeditor-wrapper">
    <div id="editor" />
  </div>
</template>
