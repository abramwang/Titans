<script lang="ts" setup>
import {
  ref,
  onMounted,
  watch,
  defineProps,
  defineEmits,
  onBeforeUnmount
} from "vue";

const emit = defineEmits(["onChange"]);

const prop = defineProps({
  config: {
    type: Object,
    default: () => {}
  },
  id: {
    type: String,
    default: "balloon-editor"
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
    classicEditor.value = "";
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
    const { default: BalloonEditor } = await import(
      "@ckeditor/ckeditor5-build-balloon"
    );
    const editorElement: HTMLElement | null = document.querySelector(
      `#${prop.id}`
    );
    if (BalloonEditor && editorElement) {
      classicEditor.value = BalloonEditor.create(
        editorElement,
        prop.config && {
          ...prop.config
        }
      )
        .then((editor: any) => {
          editorInstance = editor;

          editor.model.document.on("change:data", () => {
            if (editor) {
              editorContent.value = editor.getData();
            }
          });

          // Set default data here
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
    <div id="ballon-editor" />
  </div>
</template>
